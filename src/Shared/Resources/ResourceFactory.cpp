#include "Shared/Resource/ResourceFactory.h"

#include "Shared/Resource/Resource.h"
#include <string>
#include <memory>

/*#include "Renderer/Resources/Texture.h"
#include "UI/Resources/Font.h"
#include "Audio/Resources/Audio.h"*/
#include "Renderer/3D/Mesh/Mesh.h"
#include "Shared/Resource/ResourcesTypes.h"

std::unique_ptr<Resource> ResourceFactory::create(ResourceType type, std::string pathName) {
    switch (type) {
        /*case ResourceType::Texture:
            return std::make_unique<Texture>(pathName);
        case ResourceType::Font:
            return std::make_unique<Font>(pathName);
        case ResourceType::Audio:
            return std::make_unique<Audio>(pathName);*/
        case ResourceType::Mesh:
            return std::make_unique<Mesh>(pathName);
        default:
            return nullptr;
    }
}