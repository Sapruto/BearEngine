#pragma once

#include "RHI/Base/Core/BaseDevice.h"

class GLFWwindow;

namespace RHI::OpenGL {
    class WindowGL;
    class ResourceGL;

    class DeviceGL : public RHI::Base::BaseDevice {
    private:
        WindowGL* GetGLWindow() const;

    public:
        explicit DeviceGL(std::shared_ptr<WindowGL> win);
        
        bool Initialize() override;
        void Shutdown() override;
        void Present() override;
        
        void Clear(RHI::Base::ClearFlagsRHI flags = RHI::Base::ClearFlagsRHI::All) override;
        
        GLFWwindow* GetGLFWWindow() const;
    };
}