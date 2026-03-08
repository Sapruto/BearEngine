#pragma once

#include "Resource.h"
#include <string>

class Font : public Resource {
public:
    explicit Font(const std::string& path) : Resource(path) {}
    
    bool Load() override;
    void Unload() override;
    bool IsLoaded() const override;
};