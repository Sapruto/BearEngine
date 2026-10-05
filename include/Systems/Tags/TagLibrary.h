#pragma once

#include <cstdint>
#include <atomic>
#include <string>
#include <unordered_map>

class TagLibrary {
private:
    uint32_t value;
    static inline std::unordered_map<uint32_t, std::string> nameMap;
    
public:
    TagLibrary(uint32_t value, const std::string& name = "") 
        : value(value) {
        if (!name.empty()) {
            nameMap[value] = name;
        }
    }
    
    uint32_t value() const { return value; }
    std::string name() const {
        auto it = nameMap.find(value);
        return it != nameMap.end() ? it->second : "Unknown";
    }
    
    operator uint32_t() const { return value; }

    bool operator==(const TagLibrary& other) const {
        return value == other.value;
    }
    
    bool operator!=(const TagLibrary& other) const {
        return value != other.value;
    }
    
    struct Hash {
        size_t operator()(const TagLibrary& tag) const {
            return std::hash<uint32_t>{}(tag.value);
        }
    };
};

namespace AllTags {
    inline const TagLibrary Untagged{0, "Untagged"};
    inline const TagLibrary Enemy{1, "Enemy"};
    inline const TagLibrary Player{2, "Player"};
    
    class Type : public TagLibrary {
    private:
        static inline uint32_t nextId = 1000;
        
    public:
        explicit Type(const std::string& name) 
            : TagLibrary(nextId++, name) {}
    };
}