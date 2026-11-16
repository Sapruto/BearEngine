#pragma once

#include "Logger/Logger.h"
#include "RHI/Base/Enums.h"
#include "RHI/Base/Handles.h"
#include "RHI/Base/Executors/ExecutorQueueRHI.h"
#include "RHI/Fabric/BackendTraits.h"
#include "RHI/RuntimeRHI.h"

#include "../RenderBackendAllias.h"
#include "../Utilities/ToBytes.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

namespace Renderer::Base {
    struct ShaderHandleWithPath {
        RHI::Handles::ShaderHandle handle;
        std::string path;
    };

    class Shader {
    private:
        std::unordered_map<RHI::ShaderStage, RHI::Handles::ShaderHandle> stageToHandler;

        bool isInitialized = false;

        std::string LoadShaderContent(std::filesystem::path path) {
            std::cout << path << std::endl;

            std::ifstream file(path);

            if (!file.is_open()) {
                Logging::LoggerFacade::Error("Failed to open file: ");
                return "";
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }

        void CompileOpenGLShader(std::string shaderContent, RHI::ShaderStage stage) {
            auto& rhi = RHI::RuntimeRHI::GetInstance();
            auto* executors = static_cast<RHI::FabricRHI::ExecutorsOwnerT<Backend>*>(rhi.Executors());
            auto* resource = executors->GetResourceExecutor();

            {
                RHI::Executors::ResourceExecutor::ShaderDesc desc;
                desc.stage = stage;
                desc.bytecode = Utilities::ToBytes(shaderContent);

                const auto id = resource->AddT<RHI::Executors::ResourceExecutor::CreateShaderParams>(RHI::Executors::ResourceExecutor::CreateShaderParams{ desc })
                    .RemoveAfterExecute().MarkProcess().Commit();
                resource->SubscribeOnce(id, [this, stage](const std::vector<RHI::Executors::ResourceExecutor::ResourceExecuteResult>& results) {
                    if (results.empty()) return;
                    const auto& res = results[0];

                    if (auto* shaderRes = std::get_if<RHI::Executors::ResourceExecutor::CreateShaderResult>(&res.data)) {
                        stageToHandler[stage] = shaderRes->handle;
                    }
                    else {
                        Logging::LoggerFacade::Error("variant does not hold CreateShaderResult\n");
                    }
                });
            }
        }
        
    public:
        Shader() = default;

        void Initialize(const std::unordered_map<RHI::ShaderStage, std::filesystem::path>& stageToPath) {
            auto& rhi = RHI::RuntimeRHI::GetInstance();
            auto* queue = rhi.Queue();

            if constexpr (std::is_same_v<Backend, RHI::FabricRHI::OpenGLBackend>) {
                for (auto [stage, path] : stageToPath) {
                    std::string content = LoadShaderContent(path);
                    CompileOpenGLShader(content, stage);
                }
                queue->Execute();
            }
            else {
                Logging::LoggerFacade::Error("Unknown graphic backend type");
            }
            isInitialized = true;
        }

        void Shutdown() {
            auto& rhi = RHI::RuntimeRHI::GetInstance();
            auto* executors = static_cast<RHI::FabricRHI::ExecutorsOwnerT<Backend>*>(rhi.Executors());
            auto* resource = executors->GetResourceExecutor();

            for (auto [stage, handle] : stageToHandler) {
                resource->AddT<RHI::Executors::ResourceExecutor::DestroyShaderParams>(RHI::Executors::ResourceExecutor::DestroyShaderParams{ handle });
            }
            stageToHandler.clear();
            isInitialized = false;
        }

        RHI::Handles::ShaderHandle GetShader(RHI::ShaderStage stage) const {
            auto it = stageToHandler.find(stage);
            if (it == stageToHandler.end()) {
                Logging::LoggerFacade::Error("GetShader called before shader handle was created");
                return {};
            }
            return it->second;
        }
    };
}