#pragma once

#include "Resource.h"
#include <string>
#include <memory>

#include "ResourcesTypes.h"

class ResourceFactory {
public:
    static std::unique_ptr<Resource> create(ResourceType type, std::string pathName);
};