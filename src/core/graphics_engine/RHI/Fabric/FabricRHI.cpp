/*#include "RHI/Fabric/FabricRHI.h"

#include "RHI/OpenGL/Core/DeviceGL.h"
#include "RHI/OpenGL/Core/WindowGL.h"
#include "RHI/OpenGL/Core/ShaderGL.h"
#include "RHI/OpenGL/Core/ContextInstanceGL.h"

#include "RHI/OpenGL/Executers/BufferExecuterGL.h"
#include "RHI/OpenGL/Executers/DrawExecuterGL.h"
#include "RHI/OpenGL/Executers/SwapChainExecuterGL.h"

namespace FabricRHI {
    using BufferExecuterVariant = std::variant<std::monostate,
                                                BufferExecute::BufferExecuterGL*>;

    using DrawExecuterVariant = std::variant<std::monostate,
                                                DrawExecute::DrawExecuterGL*>;

    using SwapChainExecuterVariant = std::variant<std::monostate,
                                                SwapChainExecute::SwapChainExecuterGL*>;

    using AnyExecuterVariant = std::variant<
        std::monostate,
        BufferExecuterVariant,
        DrawExecuterVariant,
        SwapChainExecuterVariant
    >;

    BackendType BackendBaseType = BackendType::OpenGL;

    std::unique_ptr<BaseContextInstanceRHI> CreateContext(BackendType type) {
        if (type == BackendType::USE_BASE) type = BackendBaseType;

        switch (type) {
            case BackendType::OpenGL: {
                auto context = std::make_unique<ContextInstanceGL>();
                return context;
            }
            case BackendType::Vulkan:
                //TODO
                return nullptr;
            default:
                return nullptr;
        }
    }

    std::unique_ptr<BaseWindow> CreateWindow(const std::string& title,
                                             unsigned int width,
                                             unsigned int height,
                                             BackendType type) {
        if (type == BackendType::USE_BASE) type = BackendBaseType;

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

    std::unique_ptr<BaseDevice> CreateDevice(std::shared_ptr<BaseWindow> window,
                                             BackendType type) {
        if (!window) return nullptr;
        if (type == BackendType::USE_BASE) type = BackendBaseType;

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

    std::unique_ptr<BaseShader> CreateShader(const std::string& vertexPath,
                                             const std::string& fragmentPath,
                                             BackendType type) {
        if (type == BackendType::USE_BASE) type = BackendBaseType;

        switch (type) {
            case BackendType::OpenGL: {
                auto shader = std::make_unique<ShaderGL>(vertexPath, fragmentPath);
                return shader;
            }
            case BackendType::Vulkan:
                //TODO
                return nullptr;
            default:
                return nullptr;
        }
    }

    void* WindowNativeHandleToVoid(const std::any& native, BackendType type) {
        if (type == BackendType::USE_BASE) type = BackendBaseType;

        switch (type) {
            case BackendType::OpenGL: {
                if (auto* pp = std::any_cast<GLFWwindow*>(&native)) {
                    return static_cast<void*>(*pp);
                }
                return nullptr;
            }
            case BackendType::Vulkan: {
                // TODO
                return nullptr;
            }
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
            if (bufferExec) raw.push_back(bufferExec.get());
            if (drawExec) raw.push_back(drawExec.get());
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
        if (type == BackendType::USE_BASE) type = BackendBaseType;
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

    std::vector<IExecuterRHI*> ExecutersOwner::GetIExecuters() const {
        return impl->raw;
    }

    IExecuterRHI* ExecutersOwner::GetIExecuter(ExecuterTypeRHI type) const {
        for (auto* e : impl->raw) {
            if (e && e->GetExecuterType() == type) {
                return e;
            }
        }
        return nullptr;
    }

    AnyExecuterVariant ExecutersOwner::GetExecuter(ExecuterTypeRHI type, BackendType backend) const {
        if (backend == BackendType::USE_BASE) backend = impl->backend;

        switch (backend) {
            case BackendType::OpenGL: {
                switch (type) {
                    case ExecuterTypeRHI::BufferExecuter:
                        if (impl->bufferExec)    return impl->bufferExec.get();
                        return std::monostate{};
                    case ExecuterTypeRHI::DrawExecuter:
                        if (impl->drawExec)      return impl->drawExec.get();
                        return std::monostate{};
                    case ExecuterTypeRHI::SwapChainExecuter:
                        if (impl->swapChainExec) return impl->swapChainExec.get();
                        return std::monostate{};
                    default:
                        return std::monostate{};
                }
            }
            case BackendType::Vulkan: {
                // TODO
                return std::monostate{};
            }
            default:
                return std::monostate{};
        }
    }

    void ExecutersOwner::Clear() {
        impl->Clear();
    }

    bool ExecutersOwner::IsEmpty() const {
        return impl->raw.empty();
    }
}
*/