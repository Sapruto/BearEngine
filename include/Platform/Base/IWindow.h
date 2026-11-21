#pragma once

#include "Math/Vector/Vector2.h"
#include "Math/Geometry/Rect.h"

#include <any>
#include <string>
#include <functional>

namespace Platform {
    class IWindow {
    protected:
        std::any nativeHandle;
        Rect windowSize;
        
        bool isVisible{false};
        bool isFullscreen{false};
        float scale{1.0f};
        
        int framebufferWidth{0};
        int framebufferHeight{0};
        bool vSync{true};
        
        bool keys[256]{false};
        Vector2f mousePos;
        
        std::function<void(int,int)> resizeCallback;
        std::function<void()> closeCallback;
        
        bool needsResize{false};
        bool needsRedraw{false};

        void SetSize(float w, float h) { 
            windowSize = {0, 0, w, h}; 
            if (resizeCallback) resizeCallback(w, h);
        }

    public:
        IWindow() = default;
        virtual ~IWindow() = default;
        
        virtual bool Create(const std::string& title, int w, int h) = 0;
        virtual void Destroy() = 0;

        virtual void PollEvents() = 0;
        virtual void SetTitle(const std::string& title) = 0;
        virtual bool ShouldClose() const = 0;
        
        int GetWidth() const { return windowSize.width; }
        int GetHeight() const { return windowSize.height; }
        const std::any& GetNativeHandle() const { return nativeHandle; }
        std::any& GetNativeHandle() { return nativeHandle; }
        
        void SetResizeCallback(std::function<void(int,int)> cb) { resizeCallback = cb; }
        void SetCloseCallback(std::function<void()> cb) { closeCallback = cb; }
    };
}