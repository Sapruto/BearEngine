#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

#include "RHI/Base/Core/BaseDevice.h"
#include "RHI/Base/Core/BaseWindow.h"
#include "RHI/Base/Core/BaseShader.h"
#include "RHI/Base/Executers/IExecuterRHI.h"

namespace FabricRHI {
    enum class BackendType : uint8_t {
        OpenGL,
        Vulkan,
        COUNT
    };

    std::unique_ptr<BaseWindow> CreateWindow(BackendType type,
                                             const std::string& title,
                                             unsigned int width,
                                             unsigned int height);

    std::unique_ptr<BaseDevice> CreateDevice(BackendType type,
                                             std::shared_ptr<BaseWindow> window);

    std::unique_ptr<BaseShader> CreateShader(BackendType type,
                                             const std::string& vertexPath,
                                             const std::string& fragmentPath);

    class ExecutersOwner {
    private:
        struct Impl;
        std::unique_ptr<Impl> impl;

    public:
        ExecutersOwner();
        ~ExecutersOwner();

        ExecutersOwner(ExecutersOwner&&) noexcept;
        ExecutersOwner& operator=(ExecutersOwner&&) noexcept;

        ExecutersOwner(const ExecutersOwner&) = delete;
        ExecutersOwner& operator=(const ExecutersOwner&) = delete;

        void Create(BackendType type, BaseDevice& device);

        std::vector<IExecuterRHI*> GetExecuters() const;

        IExecuterRHI* GetExecuter(ExecuterTypeRHI type) const;

        void Clear();

        bool IsEmpty() const;
    };
}