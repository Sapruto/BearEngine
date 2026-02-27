#include "include/core/system_engine/resource_system/ResourceFactory.h"

#include "include/core/system_engine/resource_system/Resource.h"
#include <string>
#include <memory>

#include "include/core/system_engine/resource_system/ResourcesTypes/Texture.h"
#include "include/core/system_engine/resource_system/ResourcesTypes/Font.h"
#include "include/core/system_engine/resource_system/ResourcesTypes/Audio.h"
#include "include/core/system_engine/resource_system/ResourcesTypes.h"

std::unique_ptr<Resource> ResourceFactory::create(ResourceType type, std::string pathName) {
    switch (type) {
        case ResourceType::Texture:
            return std::make_unique<Texture>(pathName);
        case ResourceType::Font:
            return std::make_unique<Font>(pathName);
        case ResourceType::Audio:
            return std::make_unique<Audio>(pathName);
        default:
            return nullptr;
    }
}