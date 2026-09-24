#include "RHI/Fabric/FabricRHI.h"

#include "RHI/OpenGL/WindowGL.h"
#include "RHI/OpenGL/DeviceGL.h"
#include "RHI/OpenGL/ShaderGL.h"

#include "RHI/OpenGL/Executers/BufferExecuterGL.h"
#include "RHI/OpenGL/Executers/DrawExecuterGL.h"
#include "RHI/OpenGL/Executers/SwapChainExecuterGL.h"

namespace FabricRHI {
    std::unique_ptr<BaseWindow> CreateWindow(BackendType type,
                                             const std::string& title,
                                             unsigned int width,
                                             unsigned int height) {
        switch (type) {
            case BackendType::OpenGL: {
                auto window = std::make_unique<WindowGL>();
                if (!window->Create(title, width, height)) {
                    return nullptr;
                }
                return window;
            }
            case BackendType::Vulkan:
                //TODO
                return nullptr;
            default:
                return nullptr;
        }
    }

    std::unique_ptr<BaseDevice> CreateDevice(BackendType type,
                                             std::shared_ptr<BaseWindow> window) {
        if (!window) return nullptr;

        switch (type) {
            case BackendType::OpenGL: {
                auto glWindow = std::dynamic_pointer_cast<WindowGL>(window);
                if (!glWindow) return nullptr;
                return std::make_unique<DeviceGL>(std::move(glWindow));
            }
            case BackendType::Vulkan:
                //TODO
                return nullptr;
            default:
                return nullptr;
        }
    }

    std::unique_ptr<BaseShader> CreateShader(BackendType type,
                                             const std::string& vertexPath,
                                             const std::string& fragmentPath) {
        switch (type) {
            case BackendType::OpenGL: {
                auto shader = std::make_unique<ShaderGL>(vertexPath, fragmentPath);
                if (!shader->Initialize()) {
                    return nullptr;
                }
                return shader;
            }
            case BackendType::Vulkan:
                //TODO
                return nullptr;
            default:
                return nullptr;
        }
    }

    struct ExecutersOwner::Impl {
        std::unique_ptr<BufferExecute::BufferExecuterGL> bufferExec;
        std::unique_ptr<DrawExecute::DrawExecuterGL> drawExec;
        std::unique_ptr<SwapChainExecute::SwapChainExecuterGL> swapChainExec;

        std::vector<IExecuterRHI*> raw;

        void RebuildRaw() {
            raw.clear();
            if (bufferExec)    raw.push_back(bufferExec.get());
            if (drawExec)      raw.push_back(drawExec.get());
            if (swapChainExec) raw.push_back(swapChainExec.get());
        }

        void Clear() {
            raw.clear();
            bufferExec.reset();
            drawExec.reset();
            swapChainExec.reset();
        }
    };

    ExecutersOwner::ExecutersOwner()
        : impl(std::make_unique<Impl>()) {}

    ExecutersOwner::~ExecutersOwner() = default;

    ExecutersOwner::ExecutersOwner(ExecutersOwner&&) noexcept = default;
    ExecutersOwner& ExecutersOwner::operator=(ExecutersOwner&&) noexcept = default;

    void ExecutersOwner::Create(BackendType type) {
        impl->Clear();

        switch (type) {
            case BackendType::OpenGL: {
                impl->bufferExec = std::make_unique<BufferExecute::BufferExecuterGL>();
                impl->drawExec = std::make_unique<DrawExecute::DrawExecuterGL>();
                impl->swapChainExec = std::make_unique<SwapChainExecute::SwapChainExecuterGL>();
                break;
            }
            case BackendType::Vulkan: {
                //TODO
                break;
            }
            default:
                break;
        }

        impl->RebuildRaw();
    }

    std::vector<IExecuterRHI*> ExecutersOwner::GetExecuters() const {
        return impl->raw;
    }

    IExecuterRHI* ExecutersOwner::GetExecuter(ExecuterTypeRHI type) const {
        for (auto* e : impl->raw) {
            if (e && e->GetExecuterType() == type) {
                return e;
            }
        }
        return nullptr;
    }

    void ExecutersOwner::Clear() {
        impl->Clear();
    }

    bool ExecutersOwner::IsEmpty() const {
        return impl->raw.empty();
    }
}