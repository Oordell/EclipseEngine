#pragma once

#include "Eclipse.h"
#include "common_types.hpp"

namespace bomberman {

enum class WallAnimations { first, second, third, forth, fifth, sixth, seventh };
enum class WallState { idle, exploding, expired };

using eclipse::units::pixels;

class SoftWall : public EntityInterface {
public:
	explicit SoftWall(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                  const eclipse::ref<eclipse::Scene>& context);

	void on_update(au::QuantityF<au::Seconds> timestep) override;

	void destroy() override { context_->destroy_entity(entity_); }

	void on_bomb_ray_hit() override;

	CellContent on_player_interaction() override { return CellContent::soft_wall; }

	[[nodiscard]] bool has_expired() const override { return wall_state_ == WallState::expired; }

	[[nodiscard]] const eclipse::Point2D& get_position() const override { return position_; }

protected:
	void update_texture() override;

private:
	void create_entity(const std::string& entity_name = "Soft wall");

	eclipse::Point2D position_;
	eclipse::ref<eclipse::TextureSheet> texture_sheet_;
	eclipse::ref<eclipse::Scene> context_;
	SubTextureIndex texture_sheet_coordinates_ {.x = pixels(1), .y = pixels(17)};
	eclipse::Entity entity_;
	eclipse::ref<eclipse::SubTexture2D> texture_wall_;
	uint32_t current_wall_texture_index_    = 0;
	au::QuantityF<au::Seconds> blink_timer_ = au::seconds(0.F);
	WallState wall_state_ {WallState::idle};

	static constexpr eclipse::Map<WallAnimations, SubTextureIndex, 7> wall_texture_index_ {
	    {{{WallAnimations::first, {.x = pixels(1), .y = pixels(17)}},
	      {WallAnimations::second, {.x = pixels(1), .y = pixels(16)}},
	      {WallAnimations::third, {.x = pixels(2), .y = pixels(16)}},
	      {WallAnimations::forth, {.x = pixels(3), .y = pixels(16)}},
	      {WallAnimations::fifth, {.x = pixels(1), .y = pixels(15)}},
	      {WallAnimations::sixth, {.x = pixels(2), .y = pixels(15)}},
	      {WallAnimations::seventh, {.x = pixels(3), .y = pixels(15)}}}}};

	static constexpr std::array<WallAnimations, 11> wall_destruction_animation_sequence_ {
	    WallAnimations::first, WallAnimations::second, WallAnimations::first,  WallAnimations::second,
	    WallAnimations::first, WallAnimations::second, WallAnimations::third,  WallAnimations::forth,
	    WallAnimations::fifth, WallAnimations::sixth,  WallAnimations::seventh};
};

}  // namespace bomberman