#pragma once

#include <unordered_map>
#include <string>

template<typename Matrix, typename Vector, typename Color>
struct Element {
    std::unordered_map<std::string, Matrix> nameToMatrixes;
    std::unordered_map<std::string, Vector> nameToVectors;
    std::unordered_map<std::string, Color> nameToColors;
    
    size_t vertex_count = 0;
    size_t index_count = 0;
    std::string name;

    std::string settingName;
    
    void SetMatrix(const std::string& name, const Matrix& matrix) {
        nameToMatrixes[name] = matrix;
    }
    
    void SetVector(const std::string& name, const Vector& vector) {
        nameToVectors[name] = vector;
    }
    
    void SetColor(const std::string& name, const Color& color) {
        nameToColors[name] = color;
    }
    
    const Matrix* GetMatrix(const std::string& name) const {
        auto it = nameToMatrixes.find(name);
        return it != nameToMatrixes.end() ? &it->second : nullptr;
    }
    
    const Vector* GetVector(const std::string& name) const {
        auto it = nameToVectors.find(name);
        return it != nameToVectors.end() ? &it->second : nullptr;
    }
    
    const Color* GetColor(const std::string& name) const {
        auto it = nameToColors.find(name);
        return it != nameToColors.end() ? &it->second : nullptr;
    }
    
    void Clear() {
        nameToMatrixes.clear();
        nameToVectors.clear();
        nameToColors.clear();
        vertex_count = 0;
        index_count = 0;
    }
    
    bool IsEmpty() const {
        return nameToMatrixes.empty() && 
               nameToVectors.empty() && 
               nameToColors.empty();
    }
};