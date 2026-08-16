#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "Vector2.h"
#include "Vector3.h"
#include "Matrix/Matrix4x4.h"

class Shader {
private:
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;
    
    int GetUniformLocation(const std::string& name);
    std::string ReadFile(const std::string& filepath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;
    
    void Bind();
    void Unbind();
    
    void SetBool(const std::string& name, bool value);
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec2(const std::string& name, float x, float y);
    void SetVec3(const std::string& name, float x, float y, float z);
    void SetVec4(const std::string& name, float x, float y, float z, float w);
    
    void SetVec3(const std::string& name, const Vector3f& value) { SetVec3(name, value.x, value.y, value.z); }
    void SetMat4(const std::string& name, Matrix4x4f& matrix);

    unsigned int GetID() const { return m_RendererID; } 
};