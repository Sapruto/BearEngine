#pragma once

#include <string>

class Resource{
protected:
    std::string pathName;

    bool loaded = false; 

public:
    Resource(const std::string& path) : pathName(path) {}
    virtual ~Resource() = default;

    std::string GetPath() const { return pathName; }

    virtual bool Load() = 0;     
    virtual void Unload() = 0;    
    virtual bool IsLoaded() const { return loaded; } 
};