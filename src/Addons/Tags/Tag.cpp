#include "Aspects/Tags/Tag.h"

#include "Aspects/Tags/TagLibrary.h"

Tag::Tag(const TagLibrary& newTag) : tag(newTag) {}

const TagLibrary& Tag::GetTag() const { return tag; }
void Tag::SetTag(const TagLibrary& newTag) { tag = newTag; }