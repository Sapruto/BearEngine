#pragma once

#include <string>
#include <unordered_map>
#include "Matrix/Matrix4x4.h"
#include "Matrix/Matrix3x3.h"
#include "Vector.h"
#include "Vector2.h"
#include "Vector3.h"

class Shader {
private:
    unsigned int m_RendererID;
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;

    std::string ReadFile(const std::string& filepath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    int GetUniformLocation(const std::string& name);

public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void Bind() const;
    void Unbind() const;

    void SetBool(const std::string& name, bool value);
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    
    void SetVec2(const std::string& name, float x, float y);
    void SetVec2(const std::string& name, const Vector2f& vector);
    void SetVec3(const std::string& name, float x, float y, float z);
    void SetVec3(const std::string& name, const Vector3f& vector);
    void SetVec4(const std::string& name, float x, float y, float z, float w);
    
    void SetMat2(const std::string& name, const BaseMatrix<float, 2, 2>& matrix);
    void SetMat3(const std::string& name, const BaseMatrix<float, 3, 3>& matrix);
    void SetMat4(const std::string& name, const BaseMatrix<float, 4, 4>& matrix);
    
    void SetMat3(const std::string& name, const Matrix3x3f& matrix);
    void SetMat4(const std::string& name, const Matrix4x4f& matrix);

    void SetTexture(const std::string& name, unsigned int textureID, int textureUnit);

    unsigned int GetID() const { return m_RendererID; } 
};