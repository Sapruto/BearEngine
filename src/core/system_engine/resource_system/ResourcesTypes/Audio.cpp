#include "include/core/system_engine/resource_system/ResourcesTypes/Audio.h"

#include "include/core/system_engine/resource_system/Resource.h"
#include <string>

bool Audio::Load() { 
    loaded = true;
    return true;
}

void Audio::Unload() {
    loaded = false;
}

bool Audio::IsLoaded() const {
    return loaded;
}