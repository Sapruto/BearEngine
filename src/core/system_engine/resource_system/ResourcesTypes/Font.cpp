#include "include/core/system_engine/resource_system/ResourcesTypes/Font.h"

#include "include/core/system_engine/resource_system/Resource.h"
#include <string>

bool Font::Load() {
    loaded = true;
    return true;
}

void Font::Unload() {
    loaded = false;
}

bool Font::IsLoaded() const {
    return loaded;
}