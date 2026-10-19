#include "Systems/Tags/Tag.h"

#include "Systems/Tags/TagLibrary.h"

Tag::Tag(const TagLibrary& newTag) : tag(newTag) {}

const TagLibrary& Tag::GetTag() const { return tag; }
void Tag::SetTag(const TagLibrary& newTag) { tag = newTag; }