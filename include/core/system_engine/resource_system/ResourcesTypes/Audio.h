#pragma once

#include "Resource.h"
#include <string>

class Audio : public Resource {
public:
    explicit Audio(const std::string& path) : Resource(path) {}
    
    bool Load() override;
    void Unload() override;
    bool IsLoaded() const override;
};