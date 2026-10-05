#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

#include "Logger/Logger.h"

#include "ResourcesTypes.h"

class Scene;
class HierarchySystem;
class GameObject;
class Component;

namespace Serialization::Chunks {
    enum class ParseContext {
        ROOT,
        RESOURCES,
        OBJECT,
        COMPONENTS,
        COMPONENT,
        HIERARCHY,
    };

    struct ParserState {
    private:
        std::vector<ParseContext> context{ParseContext::ROOT};

    public:
        Scene* scene = nullptr;
        std::unique_ptr<GameObject> currentObject;
        Component* currentComponent = nullptr;
        std::string currentCompName;

        std::unordered_map<std::string, ResourceType> resources;

        ParseContext Current() const { return context.back(); }
        bool In(ParseContext ctx) const { return Current() == ctx; }

        void Push(ParseContext ctx) { context.push_back(ctx); }
        void Pop(ParseContext expected) {
            if (context.size() <= 1 || context.back() != expected) {
                Logging::LoggerFacade::Warn("Pop mismatch.");
                return;
            }
            context.pop_back();
        }
        void ClearContext() { if (context.size() > 1) context.pop_back(); }
    };

    struct WriteState {
    private:
        std::vector<ParseContext> context{ParseContext::ROOT};

    public:
        std::string out;
        Scene* scene = nullptr;
        GameObject* currentObject = nullptr;
        Component* currentComponent = nullptr;
        HierarchySystem* hierarchy = nullptr;

        ParseContext Current() const { return context.back(); }
        bool In(ParseContext ctx) const { return Current() == ctx; }

        void Push(ParseContext ctx) { context.push_back(ctx); }
        void Pop(ParseContext expected) {
            if (context.size() <= 1 || context.back() != expected) {
                Logging::LoggerFacade::Warn("Pop mismatch.");
                return;
            }
            context.pop_back();
        }
        void ClearContext() { if (context.size() > 1) context.pop_back(); }

        int Offset() const { return context.size() - 1; }
    };
}