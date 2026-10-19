#pragma once

#include "World/Components/Component.h"
#include "Systems/Tags/TagLibrary.h"

class Tag : public Component {
private:
    TagLibrary tag;
    
public:
    Tag(const TagLibrary& newTag = AllTags::Untagged);
    
    const TagLibrary& GetTag() const;
    void SetTag(const TagLibrary& newTag);
};