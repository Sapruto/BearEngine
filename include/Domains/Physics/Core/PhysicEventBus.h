#pragma once

#include "IPhysicEvent.h"

#include <cassert>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace Physic::Events {
    enum class EventDispatchMode {
        Immediate,
        BeginFrame,
        EndFrame
    };

    class PhysicEventBus {
    private:
        struct Subscriber {
            size_t id;
            EventDispatchMode mode;
            std::function<void(const IPhysicEvent&)> callback;
        };

        std::unordered_map<std::type_index, std::vector<std::unique_ptr<IPhysicEvent>>> typeToEvent;
        std::unordered_map<std::type_index, std::vector<Subscriber>> typeToSubscribers;

        size_t nextSubscriberId = 1;
        bool isDispatch = false;

        struct DispatchGuard {
            bool& flag;
            bool prev;
            DispatchGuard(bool& f) : flag(f), prev(f) { flag = true; }
            ~DispatchGuard() { flag = prev; }
        };

        void DispatchByMode(EventDispatchMode mode) {
            DispatchGuard guard(isDispatch);

            for (const auto& [type, events] : typeToEvent) {
                auto it = typeToSubscribers.find(type);
                if (it == typeToSubscribers.end()) continue;

                std::vector<const IPhysicEvent*> eventSnapshot;
                eventSnapshot.reserve(events.size());
                for (const auto& e : events) eventSnapshot.push_back(e.get());

                const auto subscribersCopy = it->second;

                for (const auto* event : eventSnapshot) {
                    for (const auto& subscriber : subscribersCopy) {
                        if (subscriber.mode == mode) {
                            subscriber.callback(*event);
                        }
                    }
                }
            }
        }

    public:
        template<typename T>
            requires (std::derived_from<T, IPhysicEvent>)
        T* AddEvent(std::unique_ptr<T> event) {
            assert(!isDispatch && "AddEvent during dispatch");
            if (isDispatch) return nullptr;

            T* raw = event.get();
            std::type_index key = std::type_index(typeid(T));

            typeToEvent[key].push_back(std::move(event));

            auto it = typeToSubscribers.find(key);
            if (it != typeToSubscribers.end()) {
                DispatchGuard guard(isDispatch);
                const auto subscribersCopy = it->second;
                for (const auto& subscriber : subscribersCopy) {
                    if (subscriber.mode == EventDispatchMode::Immediate) {
                        subscriber.callback(*raw);
                    }
                }
            }

            return raw;
        }

        void RemoveEvent(const IPhysicEvent* event) {
            if (event == nullptr) return;

            assert(!isDispatch && "AddEvent during dispatch");
            if (isDispatch) return;

            auto it = typeToEvent.find(std::type_index(typeid(*event)));
            if (it == typeToEvent.end()) return;

            auto& events = it->second;
            events.erase(
                std::remove_if(events.begin(), events.end(),
                    [event](const std::unique_ptr<IPhysicEvent>& e) { return e.get() == event; }),
                events.end());
        }

        template<typename T>
            requires (std::derived_from<T, IPhysicEvent>)
        size_t Subscribe(std::function<void(const T&)> callback,
                                        EventDispatchMode mode = EventDispatchMode::EndFrame) {
            assert(!isDispatch && "AddEvent during dispatch");
            if (isDispatch) return -1;

            size_t id = nextSubscriberId++;
            typeToSubscribers[std::type_index(typeid(T))].push_back(Subscriber{
                id, mode,
                [callback = std::move(callback)](const IPhysicEvent& event) {
                    callback(static_cast<const T&>(event));
                }
            });
            return id;
        }

        template<typename T>
            requires (std::derived_from<T, IPhysicEvent>)
        void Unsubscribe(size_t id) {
            assert(!isDispatch && "AddEvent during dispatch");
            if (isDispatch) return;

            auto it = typeToSubscribers.find(std::type_index(typeid(T)));
            if (it == typeToSubscribers.end()) return;

            auto& subscribers = it->second;
            subscribers.erase(
                std::remove_if(subscribers.begin(), subscribers.end(),
                    [id](const Subscriber& s) { return s.id == id; }),
                subscribers.end());
        }

        void DispatchBeginFrame() { DispatchByMode(EventDispatchMode::BeginFrame); }
        void DispatchEndFrame() { DispatchByMode(EventDispatchMode::EndFrame); }

        void ClearEvents() {
            assert(!isDispatch && "AddEvent during dispatch");
            if (isDispatch) return;
            typeToEvent.clear();
        }

        void ClearAll() {
            assert(!isDispatch && "AddEvent during dispatch");
            if (isDispatch) return;
            typeToEvent.clear();
            typeToSubscribers.clear();
        }
    };
}