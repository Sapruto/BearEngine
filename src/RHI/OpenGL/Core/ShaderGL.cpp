#include "RHI/OpenGL/Core/ShaderGL.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <glad/glad.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

using namespace RHI::OpenGL;

static constexpr GLsizei INFO_LOG_SIZE = 512;

ShaderGL::ShaderGL(const std::string& vertexPath, const std::string& fragmentPath)
    : BaseShader(vertexPath, fragmentPath) {
    CompileShader();
}

ShaderGL::~ShaderGL() {
    UncompileShader();
}

ShaderGL::ShaderGL(ShaderGL&& other) noexcept
    : BaseShader(std::move(other)),
      rendererID(other.rendererID),
      uniformLocationCache(std::move(other.uniformLocationCache)) {
    other.rendererID = 0;
    other.SetReady(false);
}

ShaderGL& ShaderGL::operator=(ShaderGL&& other) noexcept {
    if (this != &other) {
        UncompileShader();

        BaseShader::operator=(std::move(other));
        rendererID = other.rendererID;
        uniformLocationCache = std::move(other.uniformLocationCache);

        other.rendererID = 0;
        other.SetReady(false);
    }
    return *this;
}

bool ShaderGL::CompileShader() {
    UncompileShader();

    const std::string vertexSource   = ReadFile(GetVertexShaderPath());
    const std::string fragmentSource = ReadFile(GetFragmentShaderPath());

    if (vertexSource.empty() || fragmentSource.empty()) {
        std::cerr << "ERROR::SHADER::FILE_EMPTY_OR_NOT_FOUND\n";
        SetReady(false);
        return false;
    }

    const unsigned int vertexShader   = CompileShaderInternal(GL_VERTEX_SHADER, vertexSource);
    const unsigned int fragmentShader = CompileShaderInternal(GL_FRAGMENT_SHADER, fragmentSource);

    if (vertexShader == 0 || fragmentShader == 0) {
        if (vertexShader)   glDeleteShader(vertexShader);
        if (fragmentShader) glDeleteShader(fragmentShader);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n";
        SetReady(false);
        return false;
    }

    const unsigned int program = glCreateProgram();
    if (program == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        std::cerr << "ERROR::SHADER::PROGRAM_CREATION_FAILED\n";
        SetReady(false);
        return false;
    }

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success = 0;
    char infoLog[INFO_LOG_SIZE];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << '\n';
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        SetReady(false);
        return false;
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    rendererID = program;
    uniformLocationCache.clear();
    SetReady(true);
    return true;
}

void ShaderGL::UncompileShader() {
    if (rendererID != 0) {
        glDeleteProgram(rendererID);
        rendererID = 0;
    }
    uniformLocationCache.clear();
    SetReady(false);
}

std::string ShaderGL::ReadFile(const std::string& filepath) const {
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "ERROR::SHADER::FILE_NOT_OPEN: " << filepath << '\n';
        return {};
    }
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

unsigned int ShaderGL::CompileShaderInternal(unsigned int type, const std::string& source) const {
    if (source.empty()) {
        std::cerr << "ERROR::SHADER::SOURCE_EMPTY\n";
        return 0;
    }

    const unsigned int shader = glCreateShader(type);
    if (shader == 0) {
        std::cerr << "ERROR::SHADER::CREATION_FAILED\n";
        return 0;
    }

    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success = 0;
    char infoLog[INFO_LOG_SIZE];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "ERROR::SHADER::"
                  << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
                  << "::COMPILATION_FAILED\n" << infoLog << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

int ShaderGL::GetUniformLocation(const std::string& name) {
    if (rendererID == 0) {
        std::cerr << "ERROR::SHADER::PROGRAM_NOT_VALID: " << name << '\n';
        return -1;
    }

    if (auto it = uniformLocationCache.find(name); it != uniformLocationCache.end()) {
        return it->second;
    }

    const int location = glGetUniformLocation(rendererID, name.c_str());
    if (location == -1) {
        std::cerr << "WARNING::SHADER::UNIFORM_NOT_FOUND: " << name << '\n';
    }
    uniformLocationCache.emplace(name, location);
    return location;
}

void ShaderGL::SetBool(const std::string& name, bool value) {
    if (const int loc = GetUniformLocation(name); loc != -1)
        glUniform1i(loc, static_cast<int>(value));
}

void ShaderGL::SetInt(const std::string& name, int value) {
    if (const int loc = GetUniformLocation(name); loc != -1)
        glUniform1i(loc, value);
}

void ShaderGL::SetFloat(const std::string& name, float value) {
    if (const int loc = GetUniformLocation(name); loc != -1)
        glUniform1f(loc, value);
}

void ShaderGL::SetVec2(const std::string& name, const Vector2f& v) {
    if (const int loc = GetUniformLocation(name); loc != -1)
        glUniform2f(loc, v.x, v.y);
}

void ShaderGL::SetVec3(const std::string& name, const Vector3f& v) {
    if (const int loc = GetUniformLocation(name); loc != -1)
        glUniform3f(loc, v.x, v.y, v.z);
}

void ShaderGL::SetVec4(const std::string& name, const Vector4f& v) {
    if (const int loc = GetUniformLocation(name); loc != -1)
        glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void ShaderGL::SetMat2(const std::string& name, const BaseMatrix<float, 2, 2>& m) {
    if (const int loc = GetUniformLocation(name); loc != -1)
        glUniformMatrix2fv(loc, 1, GL_FALSE, m.GetData());
}

void ShaderGL::SetMat3(const std::string& name, const BaseMatrix<float, 3, 3>& m) {
    if (const int loc = GetUniformLocation(name); loc != -1)
        glUniformMatrix3fv(loc, 1, GL_FALSE, m.GetData());
}

void ShaderGL::SetMat4(const std::string& name, const BaseMatrix<float, 4, 4>& m) {
    if (const int loc = GetUniformLocation(name); loc != -1)
        glUniformMatrix4fv(loc, 1, GL_FALSE, m.GetData());
}