#pragma once

#include "Component.h"
#include "TagLibrary.h"

class Tag : public Component {
private:
    TagLibrary tag;
    
public:
    Tag(const TagLibrary& newTag);
    
    const TagLibrary& GetTag() const;
    void SetTag(const TagLibrary& newTag);
};