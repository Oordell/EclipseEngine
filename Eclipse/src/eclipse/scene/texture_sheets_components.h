#pragma once

#include "eclipse/renderer/texture.h"
#include "eclipse/renderer/sub_texture_2d.h"

#include <au.hh>

namespace eclipse::component {

struct TextureSheetComponent {
	TextureSheetComponent()                             = default;
	~TextureSheetComponent()                            = default;
	TextureSheetComponent(const TextureSheetComponent&) = default;
	ref<TextureSheet> texture_sheet                     = nullptr;
};

struct SubTexture {
	SubTexture()                  = default;
	~SubTexture()                 = default;
	SubTexture(const SubTexture&) = default;
	ref<SubTexture2D> sub_texture = nullptr;
};

}  // namespace eclipse::component