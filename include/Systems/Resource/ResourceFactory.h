#pragma once

#include "Resource.h"
#include <string>
#include <memory>

#include "Systems/Resource/ResourcesTypes.h"

class ResourceFactory {
public:
    static std::unique_ptr<Resource> create(ResourceType type, std::string pathName);
};