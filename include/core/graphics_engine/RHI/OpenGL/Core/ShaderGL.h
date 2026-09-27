#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

#include "RHI/Base/Core/BaseShader.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

class ShaderGL final : public BaseShader {
private:
    unsigned int rendererID{0};
    std::unordered_map<std::string, int> uniformLocationCache;

    std::string ReadFile(const std::string& filepath) const;
    unsigned int CompileShaderInternal(unsigned int type, const std::string& source) const;
    int GetUniformLocation(const std::string& name);

public:
    ShaderGL(const std::string& vertexPath, const std::string& fragmentPath);
    ~ShaderGL() override;

    ShaderGL(ShaderGL&& other) noexcept;
    ShaderGL& operator=(ShaderGL&& other) noexcept;

    ShaderGL(const ShaderGL&) = delete;
    ShaderGL& operator=(const ShaderGL&) = delete;

    bool CompileShader() override;
    void UncompileShader() override;

    void SetBool(const std::string& name, bool value) override;
    void SetInt(const std::string& name, int value) override;
    void SetFloat(const std::string& name, float value) override;

    void SetVec2(const std::string& name, const Vector2f& v) override;
    void SetVec3(const std::string& name, const Vector3f& v) override;
    void SetVec4(const std::string& name, const Vector4f& v) override;

    void SetMat2(const std::string& name, const BaseMatrix<float, 2, 2>& m) override;
    void SetMat3(const std::string& name, const BaseMatrix<float, 3, 3>& m) override;
    void SetMat4(const std::string& name, const BaseMatrix<float, 4, 4>& m) override;

    uint64_t GetNativeHandle() const override { return static_cast<uint64_t>(rendererID); }
};