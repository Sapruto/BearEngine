#pragma once

#include "include/core/system_engine/component_system/Component.h"
#include "include/core/system_engine/tags_system/TagLibrary.h"

class Tag : public Component {
private:
    TagLibrary tag;
    
public:
    Tag(const TagLibrary& newTag);
    
    const TagLibrary& GetTag() const;
    void SetTag(const TagLibrary& newTag);
};