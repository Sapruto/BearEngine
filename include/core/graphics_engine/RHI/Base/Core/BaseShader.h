#pragma once

#include <string>
#include <unordered_map>
#include "Matrix/Matrix4x4.h"
#include "Matrix/Matrix3x3.h"
#include "Vector2.h"
#include "Vector3.h"

class BaseShader {
private:
    bool isReady{false};

public:
    BaseShader() = default;
    BaseShader(const std::string& vs, const std::string& fs);
    virtual ~BaseShader() {}

    virtual bool CompileShader() = 0;
    virtual void UncompileShader() = 0;

    virtual void SetBool(const std::string& name, bool value) = 0;
    virtual void SetInt(const std::string& name, int value) = 0;
    virtual void SetFloat(const std::string& name, float value) = 0;
    
    virtual void SetVec2(const std::string& name, const Vector2f& vector) = 0;
    virtual void SetVec3(const std::string& name, const Vector3f& vector) = 0;
    virtual void SetVec4(const std::string& name, float x, float y, float z, float w) = 0;
    
    virtual void SetMat2(const std::string& name, const BaseMatrix<float, 2, 2>& matrix) = 0;
    virtual void SetMat3(const std::string& name, const BaseMatrix<float, 3, 3>& matrix) = 0;
    virtual void SetMat4(const std::string& name, const BaseMatrix<float, 4, 4>& matrix) = 0;

    bool IsReady() { return isReady; }
};