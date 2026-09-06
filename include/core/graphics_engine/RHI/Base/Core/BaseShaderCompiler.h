#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <concepts>
#include <cstdint>
#include <unordered_set>
#include <filesystem>
#include <optional>

template<typename T>
concept HasCompileImpl = requires(T t, const CompileContext& ctx) {
    { t.CompileImpl(ctx) } -> std::same_as<CompileResult>;
};

struct ShaderReflection {
    std::string name;
    uint32_t location;
    uint32_t binding;
    uint32_t size;
    bool isTexture;
    bool isUniformBuffer;
};

struct CompileResult {
    std::vector<uint32_t> spirv;
    std::vector<ShaderReflection> reflection;
    std::string entryPoint;
    std::string errorLog;
    size_t hash = 0;
    bool success = false;
    
    uint32_t vertexInputMask = 0;
    uint32_t pushConstantSize = 0;
};

template<typename ShaderCompilerImpl>
requires(HasCompileImpl<ShaderCompilerImpl>)
class BaseShaderCompiler {
private:
    std::vector<std::string> includePaths;
    std::unordered_map<std::string, std::string> defines;
    std::unordered_map<size_t, CompileResult> cache;
    std::string shaderSource;
    std::string entryPoint = "main";
    
    size_t CalculateHash() const {
        size_t hash = 0;
        hash ^= std::hash<std::string>{}(shaderSource) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        for (const auto& [key, val] : defines) {
            hash ^= std::hash<std::string>{}(key + "=" + val);
        }
        return hash;
    }

protected:
    struct CompileContext {
        const std::vector<std::string>& includePaths;
        const std::unordered_map<std::string, std::string>& defines;
        const std::string& entryPoint;
        const std::string& shaderSource;
    };

public:
    void SetShaderSource(const std::string& source) {
        shaderSource = source;
        cache.clear();
    }
    
    void SetEntryPoint(const std::string& name) {
        entryPoint = name;
    }
    
    void AddIncludePaths(const std::vector<std::string>& paths) {
        for (const auto& p : paths) {
            if (std::filesystem::exists(p) && std::filesystem::is_directory(p)) {
                includePaths.push_back(std::filesystem::absolute(p).string());
            }
        }
        std::unordered_set<std::string> seen;
        std::vector<std::string> unique;
        for (const auto& p : includePaths) {
            if (seen.insert(p).second) {
                unique.push_back(p);
            }
        }
        includePaths.swap(unique);
    }
    
    void AddDefine(const std::string& name, const std::string& value = "1") {
        defines[name] = value;
        cache.clear();
    }
    
    void RemoveDefine(const std::string& name) {
        defines.erase(name);
    }
    
    void ClearCache() {
        cache.clear();
    }
    
    CompileResult Compile() {
        size_t hash = CalculateHash();
        
        if (auto it = cache.find(hash); it != cache.end()) {
            return it->second;
        }
        
        CompileContext ctx{includePaths, defines, entryPoint, shaderSource};
        auto& impl = static_cast<ShaderCompilerImpl&>(*this);
        CompileResult result = impl.CompileImpl(ctx);
        
        if (result.success) {
            cache[hash] = result;
        }
        
        return result;
    }
    
    const std::vector<std::string>& GetIncludePaths() const {
        return includePaths;
    }
    
    const auto& GetDefines() const {
        return defines;
    }
};