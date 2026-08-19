#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSource = ReadFile(vertexPath);
    std::string fragmentSource = ReadFile(fragmentPath);
    
    if (vertexSource.empty() || fragmentSource.empty()) {
        std::cout << "ERROR::SHADER::FILE_EMPTY_OR_NOT_FOUND" << std::endl;
        m_RendererID = 0;
        return;
    }
    
    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    
    if (vertexShader == 0 || fragmentShader == 0) {
        std::cout << "ERROR: Shader compilation failed!" << std::endl;
        m_RendererID = 0;
        return;
    }
    
    m_RendererID = glCreateProgram();
    if (m_RendererID == 0) {
        std::cout << "ERROR::SHADER::PROGRAM_CREATION_FAILED" << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }
    
    glAttachShader(m_RendererID, vertexShader);
    glAttachShader(m_RendererID, fragmentShader);
    glLinkProgram(m_RendererID);
    
    int success;
    char infoLog[512];
    glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_RendererID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteProgram(m_RendererID);
        m_RendererID = 0;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::Shader(Shader&& other) noexcept
    : m_RendererID(other.m_RendererID),
      m_UniformLocationCache(std::move(other.m_UniformLocationCache)) {
    other.m_RendererID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (m_RendererID != 0) {
            glDeleteProgram(m_RendererID);
        }
        
        m_RendererID = other.m_RendererID;
        m_UniformLocationCache = std::move(other.m_UniformLocationCache);
        
        other.m_RendererID = 0;
    }
    return *this;
}

Shader::~Shader() {
    if (m_RendererID != 0) {
        glDeleteProgram(m_RendererID);
    }
}

std::string Shader::ReadFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "ERROR::SHADER::FILE_NOT_OPEN: " << filepath << std::endl;
        return "";
    }
    
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    return stream.str();
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    if (source.empty()) {
        std::cout << "ERROR::SHADER::SOURCE_EMPTY" << std::endl;
        return 0;
    }
    
    unsigned int shader = glCreateShader(type);
    if (shader == 0) {
        std::cout << "ERROR::SHADER::CREATION_FAILED" << std::endl;
        return 0;
    }
    
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::string shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        std::cout << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

int Shader::GetUniformLocation(const std::string& name) {
    if (m_RendererID == 0) {
        std::cout << "ERROR::SHADER::PROGRAM_NOT_VALID: " << name << std::endl;
        return -1;
    }
    
    auto it = m_UniformLocationCache.find(name);
    if (it != m_UniformLocationCache.end()) {
        return it->second;
    }
    
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1) {
        std::cout << "WARNING::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
    }
    m_UniformLocationCache[name] = location;
    return location;
}

void Shader::Bind() const {
    if (m_RendererID != 0) {
        glUseProgram(m_RendererID);
    } else {
        std::cout << "WARNING::SHADER::CANNOT_BIND_INVALID_PROGRAM" << std::endl;
    }
}

void Shader::Unbind() const {
    glUseProgram(0);
}

void Shader::SetBool(const std::string& name, bool value) {
    int location = GetUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, static_cast<int>(value));
    }
}

void Shader::SetInt(const std::string& name, int value) {
    int location = GetUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void Shader::SetFloat(const std::string& name, float value) {
    int location = GetUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void Shader::SetVec2(const std::string& name, float x, float y) {
    int location = GetUniformLocation(name);
    if (location != -1) {
        glUniform2f(location, x, y);
    }
}

void Shader::SetVec2(const std::string& name, const Vector2f& vector) {
    int location = GetUniformLocation(name);
    if (location != -1) {
        glUniform2f(location, vector.x, vector.y);
    }
}

void Shader::SetVec3(const std::string& name, float x, float y, float z) {
    int location = GetUniformLocation(name);

    if (location == -1) {
        std::cout << "ERROR::SHADER::VEC3 DATA NULL: " << name << std::endl;
        return;
    }

    if (location != -1) {
        glUniform3f(location, x, y, z);
    }
}

void Shader::SetVec3(const std::string& name, const Vector3f& vector) {
    int location = GetUniformLocation(name);

    if (location == -1) {
        std::cout << "ERROR::SHADER::VEC3 DATA NULL: " << name << std::endl;
        return;
    }

    if (location != -1) {
        glUniform3f(location, vector.x, vector.y, vector.z);
    }
}

void Shader::SetVec4(const std::string& name, float x, float y, float z, float w) {
    int location = GetUniformLocation(name);

    if (location == -1) {
        std::cout << "ERROR::SHADER::VEC4 DATA NULL: " << name << std::endl;
        return;
    }

    if (location != -1) {
        glUniform4f(location, x, y, z, w);
    }
}

void Shader::SetMat2(const std::string& name, const BaseMatrix<float, 2, 2>& matrix) {
    int location = GetUniformLocation(name);
    if (location == -1) {
        return;
    }
    
    const float* data = matrix.GetData();
    if (data == nullptr) {
        std::cout << "ERROR::SHADER::MATRIX_DATA_NULL: " << name << std::endl;
        return;
    }
    
    glUniformMatrix2fv(location, 1, GL_FALSE, matrix.GetData());
}

void Shader::SetMat3(const std::string& name, const BaseMatrix<float, 3, 3>& matrix) {
    int location = GetUniformLocation(name);
    if (location == -1) {
        return;
    }
    
    const float* data = matrix.GetData();
    if (data == nullptr) {
        std::cout << "ERROR::SHADER::MATRIX_DATA_NULL: " << name << std::endl;
        return;
    }

    glUniformMatrix3fv(location, 1, GL_FALSE, matrix.GetData());
}

void Shader::SetMat4(const std::string& name, const BaseMatrix<float, 4, 4>& matrix) {
    int location = GetUniformLocation(name);
    if (location == -1) {
        return;
    }
    
    const float* data = matrix.GetData();
    if (data == nullptr) {
        std::cout << "ERROR::SHADER::MATRIX_DATA_NULL: " << name << std::endl;
        return;
    }
    
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix.GetData());
}

void Shader::SetMat4(const std::string& name, const Matrix4x4f& matrix) {
    SetMat4(name, static_cast<const BaseMatrix<float, 4, 4>&>(matrix));
}

void Shader::SetMat3(const std::string& name, const Matrix3x3f& matrix) {
    SetMat3(name, static_cast<const BaseMatrix<float, 3, 3>&>(matrix));
}

void Shader::SetTexture(const std::string& name, unsigned int textureID, int textureUnit = 0) {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
    SetInt(name, textureUnit);
}