#include "Audio.h"

#include "Resource.h"
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