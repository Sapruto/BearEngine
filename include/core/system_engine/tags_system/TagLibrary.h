#pragma once

#include <cstdint>
#include <atomic>
#include <string>
#include <unordered_map>

class TagLibrary {
private:
    uint32_t value_;
    static inline std::unordered_map<uint32_t, std::string> nameMap_;
    
public:
    TagLibrary(uint32_t value, const std::string& name = "") 
        : value_(value) {
        if (!name.empty()) {
            nameMap_[value] = name;
        }
    }
    
    uint32_t value() const { return value_; }
    std::string name() const {
        auto it = nameMap_.find(value_);
        return it != nameMap_.end() ? it->second : "Unknown";
    }
    
    operator uint32_t() const { return value_; }

    bool operator==(const TagLibrary& other) const {
        return value_ == other.value_;
    }
    
    bool operator!=(const TagLibrary& other) const {
        return value_ != other.value_;
    }
    
    struct Hash {
        size_t operator()(const TagLibrary& tag) const {
            return std::hash<uint32_t>{}(tag.value_);
        }
    };
};

namespace AllTags {
    inline const TagLibrary Untagged{0, "Untagged"};
    inline const TagLibrary Enemy{1, "Enemy"};
    inline const TagLibrary Player{2, "Player"};
    
    class Type : public TagLibrary {
    private:
        static inline uint32_t nextId_ = 1000;
        
    public:
        explicit Type(const std::string& name) 
            : TagLibrary(nextId_++, name) {}
    };
}