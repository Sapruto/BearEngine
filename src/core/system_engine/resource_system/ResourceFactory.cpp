#include "ResourceFactory.h"

#include "Resource.h"
#include <string>
#include <memory>

#include "Texture.h"
#include "Font.h"
#include "Audio.h"
#include "ResourcesTypes.h"

std::unique_ptr<Resource> ResourceFactory::create(ResourceType type, std::string pathName) {
    switch (type) {
        case ResourceType::Texture:
            return std::make_unique<Texture>(pathName);
        case ResourceType::Font:
            return std::make_unique<Font>(pathName);
        case ResourceType::Audio:
            return std::make_unique<Audio>(pathName);
        case ResourceType::Model:
            return std::make_unique<Model>(pathName);
        default:
            return nullptr;
    }
}