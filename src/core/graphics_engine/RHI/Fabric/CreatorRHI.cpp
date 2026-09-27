#include "RHI/Fabric/CreatorRHI.h"

#include "RHI/Fabric/BackendTraits.h"
#include "RHI/Fabric/ExecutersOwnerT.h"

#include "RHI/OpenGL/Core/ContextInstanceGL.h"
#include "RHI/OpenGL/Core/WindowGL.h"
#include "RHI/OpenGL/Core/DeviceGL.h"
#include "RHI/OpenGL/Core/ShaderGL.h"

#include <GLFW/glfw3.h>

namespace FabricRHI {
    using namespace BufferExecute;
    using namespace DrawExecute;
    using namespace SwapChainExecute;

    template<template<typename> class Fn, typename... Args>
    auto Dispatch(BackendType type, Args&&... args)
        -> decltype(Fn<OpenGLBackend>::Run(std::forward<Args>(args)...))
    {
        switch (type) {
            case BackendType::OpenGL:
                return Fn<OpenGLBackend>::Run(std::forward<Args>(args)...);

            case BackendType::Vulkan:
                // TODO: return Fn<VulkanBackend>::Run(...);
                return Fn<OpenGLBackend>::Run(std::forward<Args>(args)...);

            default:
                return Fn<OpenGLBackend>::Run(std::forward<Args>(args)...);
        }
    }

    template<typename Backend>
    struct CreateContextFn {
        static std::unique_ptr<BaseContextInstanceRHI> Run() {
            return std::make_unique<typename Backend::ContextInstance>();
        }
    };

    template<typename Backend>
    struct CreateWindowFn {
        static std::unique_ptr<BaseWindow> Run(const std::string& title,
                                            unsigned int w,
                                            unsigned int h) {
            auto window = std::make_unique<typename Backend::Window>();
            if (!window->Create(title, w, h)) return nullptr;
            return window;
        }
    };

    template<typename Backend>
    struct CreateDeviceFn {
        static std::unique_ptr<BaseDevice> Run(std::shared_ptr<BaseWindow> window) {
            auto native = std::dynamic_pointer_cast<typename Backend::Window>(window);
            if (!native) return nullptr;
            return std::make_unique<typename Backend::Device>(std::move(native));
        }
    };

    template<typename Backend>
    struct CreateShaderFn {
        static std::unique_ptr<BaseShader> Run(const std::string& vs, const std::string& fs) {
            return std::make_unique<typename Backend::Shader>(vs, fs);
        }
    };

    template<typename Backend>
    struct CreateExecutersFn {
        static std::unique_ptr<IExecutersOwner> Run() {
            auto owner = std::make_unique<ExecutersOwnerT<Backend>>();
            owner->Create(Backend::kType);
            return owner;
        }
    };


    CreatorRHI::CreatorRHI(BackendType type)
        : backend(type == BackendType::USE_BASE ? BackendType::OpenGL : type) {}

    CreatorRHI::~CreatorRHI() = default;
    CreatorRHI::CreatorRHI(CreatorRHI&&) noexcept = default;
    CreatorRHI& CreatorRHI::operator=(CreatorRHI&&) noexcept = default;

    void CreatorRHI::SetBackend(BackendType type) {
        backend = (type == BackendType::USE_BASE) ? BackendType::OpenGL : type;
    }

    BackendType CreatorRHI::GetBackend() const {
        return backend;
    }

    std::unique_ptr<BaseContextInstanceRHI> CreatorRHI::CreateContext() const {
        return Dispatch<CreateContextFn>(backend);
    }

    std::unique_ptr<BaseWindow> CreatorRHI::CreateWindow(const std::string& title,
                                                        unsigned int width,
                                                        unsigned int height) const {
        return Dispatch<CreateWindowFn>(backend, title, width, height);
    }

    std::unique_ptr<BaseDevice> CreatorRHI::CreateDevice(std::shared_ptr<BaseWindow> window) const {
        return Dispatch<CreateDeviceFn>(backend, std::move(window));
    }

    std::unique_ptr<BaseShader> CreatorRHI::CreateShader(const std::string& vertexPath,
                                                        const std::string& fragmentPath) const {
        return Dispatch<CreateShaderFn>(backend, vertexPath, fragmentPath);
    }

    std::unique_ptr<IExecutersOwner> CreatorRHI::CreateExecuters() const {
        return Dispatch<CreateExecutersFn>(backend);
    }

    void* CreatorRHI::WindowNativeHandleToVoid(const std::any& native) const {
        switch (backend) {
            case BackendType::OpenGL:
                if (auto* pp = std::any_cast<GLFWwindow*>(&native))
                    return static_cast<void*>(*pp);
                return nullptr;
            default:
                return nullptr;
        }
    }
}