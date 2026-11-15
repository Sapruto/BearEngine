#include "Systems/Resource/ResourceFactory.h"

#include "Systems/Resource/Resource.h"
#include <string>
#include <memory>

/*#include "Domains/Renderer/Resources/Texture.h"
#include "UI/Resources/Font.h"
#include "Audio/Resources/Audio.h"*/
#include "Domains/Renderer/3D/Mesh/Mesh.h"
#include "Systems/Resource/ResourcesTypes.h"

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