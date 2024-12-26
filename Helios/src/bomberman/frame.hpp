#pragma once

#include "Eclipse.h"
#include "common_types.hpp"

namespace bomberman {

using eclipse::units::pixels;

class Frame : public EntityInterface {
public:
	explicit Frame(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	               const eclipse::ref<eclipse::Scene>& context, const SubTextureIndex& texture_sheet_coordinates);

	void on_update(au::QuantityF<au::Seconds> timestep) override {}

	void destroy() override {}

	void on_bomb_ray_hit() override {}

	CellContent on_player_interaction() override { return CellContent::wall_solid; }

	[[nodiscard]] bool has_expired() const override { return false; }

	[[nodiscard]] virtual const eclipse::Point2D& get_position() const override { return position_; }

protected:
	void update_texture() override {};

private:
	void create_entity(const std::string& entity_name = "Frame");

	eclipse::Point2D position_;
	eclipse::ref<eclipse::TextureSheet> texture_sheet_;
	eclipse::ref<eclipse::Scene> context_;
	SubTextureIndex texture_sheet_coordinates_;
	eclipse::Entity entity_;
	eclipse::ref<eclipse::SubTexture2D> texture_frame_;
};

class FrameCornerTopRight : public Frame {
public:
	explicit FrameCornerTopRight(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                             const eclipse::ref<eclipse::Scene>& context,
	                             au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(4) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(23)}) {}
};

class FrameCornerTopLeft : public Frame {
public:
	explicit FrameCornerTopLeft(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                            const eclipse::ref<eclipse::Scene>& context,
	                            au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(0) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(23)}) {}
};

class FrameCornerBottomRight : public Frame {
public:
	explicit FrameCornerBottomRight(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                                const eclipse::ref<eclipse::Scene>& context,
	                                au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(4) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(19)}) {}
};

class FrameCornerBottomLeft : public Frame {
public:
	explicit FrameCornerBottomLeft(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                               const eclipse::ref<eclipse::Scene>& context,
	                               au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(0) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(19)}) {}
};

class FrameLeft1 : public Frame {
public:
	explicit FrameLeft1(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                    const eclipse::ref<eclipse::Scene>& context,
	                    au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(0) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(22)}) {}
};

class FrameLeft2 : public Frame {
public:
	explicit FrameLeft2(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                    const eclipse::ref<eclipse::Scene>& context,
	                    au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(0) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(21)}) {}
};

class FrameLeft3 : public Frame {
public:
	explicit FrameLeft3(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                    const eclipse::ref<eclipse::Scene>& context,
	                    au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(0) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(20)}) {}
};

class FrameRight1 : public Frame {
public:
	explicit FrameRight1(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                     const eclipse::ref<eclipse::Scene>& context,
	                     au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(4) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(22)}) {}
};

class FrameRight2 : public Frame {
public:
	explicit FrameRight2(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                     const eclipse::ref<eclipse::Scene>& context,
	                     au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(4) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(21)}) {}
};

class FrameRight3 : public Frame {
public:
	explicit FrameRight3(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                     const eclipse::ref<eclipse::Scene>& context,
	                     au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(4) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(20)}) {}
};

class FrameTop1 : public Frame {
public:
	explicit FrameTop1(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                   const eclipse::ref<eclipse::Scene>& context,
	                   au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(1) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(23)}) {}
};

class FrameTop2 : public Frame {
public:
	explicit FrameTop2(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                   const eclipse::ref<eclipse::Scene>& context,
	                   au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(2) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(23)}) {}
};

class FrameTop3 : public Frame {
public:
	explicit FrameTop3(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                   const eclipse::ref<eclipse::Scene>& context,
	                   au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(3) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(23)}) {}
};

class FrameBottom1 : public Frame {
public:
	explicit FrameBottom1(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                      const eclipse::ref<eclipse::Scene>& context,
	                      au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(1) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(19)}) {}
};

class FrameBottom2 : public Frame {
public:
	explicit FrameBottom2(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                      const eclipse::ref<eclipse::Scene>& context,
	                      au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(2) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(19)}) {}
};

class FrameBottom3 : public Frame {
public:
	explicit FrameBottom3(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                      const eclipse::ref<eclipse::Scene>& context,
	                      au::Quantity<eclipse::units::Pixels, uint32_t> level_type)
	    : Frame(texture_sheet, position, context,
	            {.x = eclipse::units::pixels(3) + level_type * details::texture_sheet_level_separator_,
	             .y = eclipse::units::pixels(19)}) {}
};

}  // namespace bomberman