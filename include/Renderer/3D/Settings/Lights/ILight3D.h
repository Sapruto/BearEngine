#pragma once

#include "World/Components/Component.h"
#include "Renderer/3D/Settings/Lights/LightDataVariant.h"

class ILight3D : public Component {
public:
    ILight3D() = default;
    virtual ~ILight3D() = default;
    
    virtual LightData GetLightData() const = 0;
};