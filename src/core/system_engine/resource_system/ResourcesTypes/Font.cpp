#include "Font.h"

#include "Resource.h"
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