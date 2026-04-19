#include "Tag.h"

#include "Component.h"
#include "TagLibrary.h"

Tag::Tag(const TagLibrary& newTag = AllTags::Untagged) : tag(newTag) {}

const TagLibrary& Tag::GetTag() const { return tag; }
void Tag::SetTag(const TagLibrary& newTag) { tag = newTag; }