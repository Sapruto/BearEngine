#pragma once

#include "include/core/system_engine/resource_system/Resource.h"
#include <string>
#include <memory>

#include "include/core/system_engine/resource_system/ResourcesTypes/Texture.h"
#include "include/core/system_engine/resource_system/ResourcesTypes/Font.h"
#include "include/core/system_engine/resource_system/ResourcesTypes/Audio.h"
#include "include/core/system_engine/resource_system/ResourcesTypes.h"

class ResourceFactory {
public:
    static std::unique_ptr<Resource> create(ResourceType type, std::string pathName);
};