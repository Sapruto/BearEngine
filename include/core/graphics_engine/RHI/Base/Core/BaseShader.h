#pragma once

#include <string>
#include <cstdint>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix/Matrix4x4.h"
#include "Matrix/Matrix3x3.h"

class BaseShader {
private:
    bool isReady{false};
    std::string vertexShaderPath;
    std::string fragmentShaderPath;

protected:
    void SetReady(bool value) { isReady = value; }

    const std::string& GetVertexShaderPath() const { return vertexShaderPath; }
    const std::string& GetFragmentShaderPath() const { return fragmentShaderPath; }

    BaseShader() = default;
    BaseShader(std::string vs, std::string fs)
        : vertexShaderPath(std::move(vs)), fragmentShaderPath(std::move(fs)) {}

public:
    virtual ~BaseShader() = default;

    BaseShader(const BaseShader&) = delete;
    BaseShader& operator=(const BaseShader&) = delete;

    BaseShader(BaseShader&&) noexcept = default;
    BaseShader& operator=(BaseShader&&) noexcept = default;

    virtual bool CompileShader() = 0;
    virtual void UncompileShader() = 0;

    virtual void SetBool(const std::string& name, bool value) = 0;
    virtual void SetInt(const std::string& name, int value) = 0;
    virtual void SetFloat(const std::string& name, float value) = 0;

    virtual void SetVec2(const std::string& name, const Vector2f& v) = 0;
    virtual void SetVec3(const std::string& name, const Vector3f& v) = 0;
    virtual void SetVec4(const std::string& name, const Vector4f& v) = 0;

    virtual void SetMat2(const std::string& name, const BaseMatrix<float, 2, 2>& m) = 0;
    virtual void SetMat3(const std::string& name, const BaseMatrix<float, 3, 3>& m) = 0;
    virtual void SetMat4(const std::string& name, const BaseMatrix<float, 4, 4>& m) = 0;

    virtual uint64_t GetNativeHandle() const = 0;

    bool IsReady() const { return isReady; }
};