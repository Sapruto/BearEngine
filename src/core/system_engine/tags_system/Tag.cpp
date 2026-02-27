#include "include/core/system_engine/tags_system/Tag.h"

#include "include/core/system_engine/component_system/Component.h"
#include "include/core/system_engine/tags_system/TagLibrary.h"

Tag::Tag(const TagLibrary& newTag = AllTags::Untagged) : tag(newTag) {}

const TagLibrary& Tag::GetTag() const { return tag; }
void Tag::SetTag(const TagLibrary& newTag) { tag = newTag; }