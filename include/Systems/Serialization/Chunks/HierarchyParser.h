#pragma once

#include "Systems/Serialization/Chunks/IChankParser.h"
#include "Systems/Serialization/Chunks/ParsingContext.h"
#include "Systems/Serialization/Chunks/ChunkTokens.h"

#include "Scene.h"
#include "HierarchySystem.h"

namespace Serialization::Chunks {
    class HierarchyParser final : public IChankParser {
    private:
        struct RawLink {
            std::vector<uint32_t> parents;
            std::vector<uint32_t> children;
        };

        std::unordered_map<uint32_t, RawLink> rawLinks;

        std::vector<uint32_t> ParseIdList(const std::string& str) {
            std::vector<uint32_t> result;
            if (str.empty()) return result;

            size_t start = 0;
            while (start < str.size()) {
                size_t comma = str.find(ChunkTokens::Hierarchy::listSep, start);
                std::string idStr = (comma == std::string::npos)
                    ? str.substr(start)
                    : str.substr(start, comma - start);

                if (!idStr.empty()) {
                    try {
                        result.push_back(static_cast<uint32_t>(std::stoul(idStr)));
                    }
                    catch (...) {
                    }
                }

                if (comma == std::string::npos) break;
                start = comma + 1;
            }
            return result;
        }

        static std::string IdsToString(const std::vector<GameObject*>& objects,
                                       const std::vector<GameObject*>& allObjects) {
            std::string result;
            bool first = true;
            for (GameObject* obj : objects) {
                if (!obj) continue;
                auto it = std::find(allObjects.begin(), allObjects.end(), obj);
                if (it == allObjects.end()) continue;

                if (!first) result += ChunkTokens::Hierarchy::listSep;
                result += std::to_string(static_cast<uint32_t>(
                    std::distance(allObjects.begin(), it)));
                first = false;
            }
            return result;
        }

        static GameObject* ObjectById(Scene* scene, uint32_t id) {
            if (!scene) return nullptr;
            auto objects = scene->GetGameObjects();
            if (id >= objects.size()) return nullptr;
            return const_cast<GameObject*>(objects[id]);
        }

    public:
        HierarchyParser() {
            layer = ParsingLayers::HIERARCHY;
        }
        ~HierarchyParser() = default;

        std::string_view Token() const override {
            return ChunkTokens::Hierarchy::hierarchy;
        }

        void OnOpen(const std::string& token, ParserState& state) override {
            state.Push(ParseContext::HIERARCHY);
            rawLinks.clear();
        }

        void OnKeyValue(const std::string& key, const std::string& value, ParserState& state) override {
            if (!state.In(ParseContext::HIERARCHY)) return;

            uint32_t objectId = 0;
            try {
                objectId = static_cast<uint32_t>(std::stoul(key));
            }
            catch (...) {
                return;
            }

            auto sep = value.find(ChunkTokens::Hierarchy::sectionSep);
            std::string parentsStr = (sep == std::string::npos) ? value : value.substr(0, sep);
            std::string childrenStr = (sep == std::string::npos) ? ""    : value.substr(sep + 1);

            RawLink link;
            link.parents = ParseIdList(parentsStr);
            link.children = ParseIdList(childrenStr);

            rawLinks[objectId] = std::move(link);
        }

        void OnClose(ParserState& state) override {
            state.Pop(ParseContext::HIERARCHY);
        }

        void OnFinalize(ParserState& state) override {
            if (!state.scene) return;

            HierarchySystem* hierarchy = state.scene->GetHierarchySystem();
            if (!hierarchy) return;

            for (auto& [objectId, link] : rawLinks) {
                GameObject* object = ObjectById(state.scene, objectId);
                if (!object) continue;

                if (!link.parents.empty()) {
                    std::vector<GameObject*> parents;
                    parents.reserve(link.parents.size());
                    for (uint32_t pid : link.parents) {
                        if (GameObject* p = ObjectById(state.scene, pid)) parents.push_back(p);
                    }
                    if (!parents.empty()) hierarchy->AddCommunication(object, parents, false);
                }

                if (!link.children.empty()) {
                    std::vector<GameObject*> children;
                    children.reserve(link.children.size());
                    for (uint32_t cid : link.children) {
                        if (GameObject* c = ObjectById(state.scene, cid)) children.push_back(c);
                    }
                    if (!children.empty()) hierarchy->AddCommunication(object, children, true);
                }
            }

            rawLinks.clear();
        }

        void OpenWrite(WriteState& state) const override {
            state.out += MakeTabs(state.Offset()) + std::string(ChunkTokens::Hierarchy::hierarchy)
                    + std::string(ChunkTokens::sep) + std::string(ChunkTokens::start_part) + "\n";

            if (!state.hierarchy || !state.scene) return;
            state.Push(ParseContext::HIERARCHY);

            auto allObjects = state.scene->GetGameObjects();

            for (size_t i = 0; i < allObjects.size(); ++i) {
                GameObject* obj = const_cast<GameObject*>(allObjects[i]);
                if (!obj) continue;

                std::vector<GameObject*> parents  = state.hierarchy->GetParents(obj);
                std::vector<GameObject*> children = state.hierarchy->GetChilds(obj);

                if (parents.empty() && children.empty()) continue;

                std::string line;
                line += std::to_string(static_cast<uint32_t>(i));
                line += ChunkTokens::Hierarchy::keyValueSep;
                line += IdsToString(parents, allObjects);
                line += ChunkTokens::Hierarchy::sectionSep;
                line += IdsToString(children, allObjects);
                line += std::string(ChunkTokens::over);

                state.out += line;
            }
        }

        void CloseWrite(WriteState& state) const override {
            state.out += MakeTabs(state.Offset()) + std::string(ChunkTokens::end_part) + std::string(ChunkTokens::over) + "\n";
            state.Pop(ParseContext::HIERARCHY);
        }
    };
}