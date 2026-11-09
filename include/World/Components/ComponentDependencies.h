#pragma once

#include <cassert>
#include <typeindex>
#include <vector>
#include <unordered_map>

#include "Component.h"

namespace World::Components::Dependencies {
    enum class ConstraintType {
        MUST_HAVE,
        MUST_NOT_HAVE,
        CONDITION
    };

    struct DependencyRule {
        ConstraintType type;
        std::type_index targetType = typeid(void);
        std::size_t minCount = 0;
        std::size_t maxCount = std::numeric_limits<std::size_t>::max();
        int priority = 0;
        bool isAutoSpawned = false;
    };

    class DependentComponents {
    private:
        std::vector<DependencyRule> rules;
        bool consumed = false;

    protected:
        template<class T>
        void AddRule(ConstraintType type,
                        std::size_t minCount = 1,
                        std::size_t maxCount = std::numeric_limits<std::size_t>::max(),
                        int priority = 100) {
            static_assert(std::is_base_of_v<Component, T>, "T must be derived from Component");
            assert(!consumed && "RequiredComponents already taken");
            rules.push_back(DependencyRule{
                type, std::type_index(typeid(T)), minCount, maxCount, priority
            });
        }

    public:
        std::vector<DependencyRule> TakeRules() {
            if (consumed) return {};
            consumed = true;
            return std::exchange(rules, {});
        }

        bool IsConsumed() const { return consumed; }
    };

    enum class ActionType { Spawn, Destroy, MoveToPendingRemove };
    struct Order {
        ActionType action;
        std::type_index componentType;
        Component* exactInstance = nullptr;
    };
    using ExecutionPlan = std::vector<Order>;

    class ComponentDependencyGraph {
    public:
        struct PendingRemoval {
            Component* componentToKill;
            std::unordered_set<std::type_index> blockingTypes; 
        };

        struct ValidationResult {
            bool success = true;
            std::string logMessage = "Success";
            std::vector<std::string> conflictReasonChain; 
        };

    private:
        std::unordered_map<Component*, std::vector<DependencyRule>> componentRules;
        std::unordered_map<std::type_index, std::size_t> activeCounts;
        std::vector<PendingRemoval> pendingRemovals;

        ValidationResult lastValidationResult;
        ExecutionPlan lastGeneratedPlan;

        bool ValidateNewComponentInRules(DependentComponents* dependencies, std::type_index newType) {
            lastValidationResult = ValidationResult{};
            lastGeneratedPlan.clear();

            DependentComponents tempCopy = *dependencies;
            auto newRules = tempCopy.TakeRules();

            for (const auto& rule : newRules) {
                if (rule.type == ConstraintType::MUST_NOT_HAVE && activeCounts[rule.targetType] > 0) {
                    for (const auto& [activeComp, activeRules] : componentRules) {
                        if (typeid(*activeComp) == rule.targetType) {
                            if (rule.priority > 100) { 
                                lastGeneratedPlan.push_back({ ActionType::Destroy, rule.targetType, activeComp });
                                lastValidationResult.conflictReasonChain.push_back(
                                    std::string("New ") + newType.name() + " evicts active " + rule.targetType.name() + " due to priority."
                                );
                            } else {
                                lastValidationResult.success = false;
                                lastValidationResult.logMessage = std::string("Addition rejected: Active ") + 
                                                                    rule.targetType.name() + " has higher or equal priority.";
                                return false;
                            }
                        }
                    }
                }
            }

            for (const auto& [activeComp, activeRules] : componentRules) {
                for (const auto& rule : activeRules) {
                    if (rule.type == ConstraintType::MUST_NOT_HAVE && rule.targetType == newType) {
                        if (100 > rule.priority) {
                            lastGeneratedPlan.push_back({ ActionType::Destroy, typeid(*activeComp), activeComp });
                            lastValidationResult.conflictReasonChain.push_back(
                                std::string("New ") + newType.name() + " overrides and kills active " + typeid(*activeComp).name()
                            );
                        } else {
                            lastValidationResult.success = false;
                            lastValidationResult.logMessage = std::string("Addition rejected: Active ") + 
                                                                typeid(*activeComp).name() + " prohibits adding " + newType.name();
                            return false;
                        }
                    }
                }
            }

            for (const auto& rule : newRules) {
                if (rule.type == ConstraintType::MUST_HAVE) {
                    std::size_t current = activeCounts[rule.targetType];
                    if (current < rule.minCount) {
                        std::size_t diff = rule.minCount - current;
                        for (std::size_t i = 0; i < diff; ++i) {
                            lastGeneratedPlan.push_back({ ActionType::Spawn, rule.targetType, nullptr });
                        }
                        lastValidationResult.conflictReasonChain.push_back(
                            std::string("Scheduled cascade spawning of ") + std::to_string(diff) + " " + rule.targetType.name()
                        );
                    }
                }
            }

            return true;
        }

        ExecutionPlan GetExecutionPlanForNewComponent(DependentComponents* dependencies) {
            return std::move(lastGeneratedPlan);
        }

    public:
        ComponentDependencyGraph() = default;

        template<typename T>
        void RegisterComponent(T* component) {
            std::type_index compType(typeid(*component));
            activeCounts[compType]++;

            if constexpr (std::is_base_of_v<DependentComponents, T>) {
                componentRules[component] = component->TakeRules();
            }
        }

        void UnregisterComponent(Component* component) {
            std::type_index compType(typeid(*component));
            if (activeCounts[compType] > 0) {
                activeCounts[compType]--;
            }
            componentRules.erase(component);

            auto it = std::remove_if(pendingRemovals.begin(), pendingRemovals.end(), 
                [this](PendingRemoval& pr) {
                    if (this->CanRemove(pr.componentToKill)) {
                        std::cout << "[DependencyGraph] Delayed removal activated for: " 
                                    << typeid(*pr.componentToKill).name() << "\n";
                        return true; 
                    }
                    return false;
                });
            pendingRemovals.erase(it, pendingRemovals.end());
        }

        template<typename T>
        ExecutionPlan OnAddComponent(T* component) {
            if constexpr (!std::is_base_of_v<DependentComponents, T>) return {};

            std::type_index newType(typeid(*component));
            auto* dependencies = static_cast<DependentComponents*>(component);
            
            if (!ValidateNewComponentInRules(dependencies, newType)) {
                std::cout << "[DependencyGraph][Warning] " << lastValidationResult.logMessage << "\n";
                return {};
            }

            for (const auto& step : lastValidationResult.conflictReasonChain) {
                std::cout << "[DependencyGraph][Info] " << step << "\n";
            }

            return GetExecutionPlanForNewComponent(dependencies);
        }

        bool CanRemove(Component* component) {
            std::type_index compType(typeid(*component));

            for (const auto& [activeComp, rules] : componentRules) {
                if (activeComp == component) continue;

                for (const auto& rule : rules) {
                    if (rule.type == ConstraintType::MUST_HAVE && rule.targetType == compType) {
                        if (activeCounts[compType] - 1 < rule.minCount) {
                            return false; 
                        }
                    }
                }
            }
            return true;
        }

        void RequestRemove(Component* component) {
            if (CanRemove(component)) {
                UnregisterComponent(component);
            }
            else {
                PendingRemoval pr{ component, {} };
                std::type_index compType(typeid(*component));
                
                for (const auto& [activeComp, rules] : componentRules) {
                    for (const auto& rule : rules) {
                        if (rule.type == ConstraintType::MUST_HAVE && rule.targetType == compType) {
                            pr.blockingTypes.insert(typeid(*activeComp));
                        }
                    }
                }
                pendingRemovals.push_back(std::move(pr));
            }
        }
    };
}