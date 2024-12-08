#pragma once

#include "Eclipse.h"
#include "common_types.hpp"

namespace bomberman {

enum class BombState { placed, exploded, expired };
enum class BombPlacedAnimation { big, medium, small };
enum class BombExplodedAnimation { small, medium, big, biggest };

using eclipse::units::pixels;
using OnExplosionCallback = std::function<void(const eclipse::Point2D& position, uint32_t reach)>;

class Bomb : public EntityInterface {
public:
	explicit Bomb(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	              const eclipse::ref<eclipse::Scene>& context, const OnExplosionCallback& on_explosion_callback,
	              uint32_t reach = 1);

	~Bomb() = default;

	void on_update(au::QuantityF<au::Seconds> timestep) override;
	void on_bomb_ray_hit() override;

	CellContent on_player_interaction() override { return CellContent::bomb; };

	[[nodiscard]] bool has_expired() const override { return bomb_state_ == BombState::expired; }

	void destroy() override { context_->destroy_entity(entity_); }

	[[nodiscard]] bool bomb_is_exploding() const { return bomb_state_ == BombState::exploded; }

	[[nodiscard]] bool bomb_is_placed() const { return bomb_state_ == BombState::placed; }

	[[nodiscard]] const eclipse::Point2D& get_position() const override { return position_; }

protected:
	void update_texture() override;

private:
	void create_entity(const std::string& entity_name = "Bomb");
	void bomb_explosion();

	eclipse::Point2D position_;
	eclipse::ref<eclipse::TextureSheet> texture_sheet_;
	eclipse::ref<eclipse::Scene> context_;
	SubTextureIndex texture_sheet_coordinates_;
	eclipse::Entity entity_;
	eclipse::ref<eclipse::SubTexture2D> texture_bomb_;
	OnExplosionCallback callback_;

	au::QuantityF<au::Seconds> blink_timer_     = au::seconds(0.F);
	au::QuantityF<au::Seconds> lifetime_timer_  = au::seconds(0.F);
	uint32_t sprite_animation_sequence_counter_ = 0;
	uint32_t bomb_reach_                        = 1;
	BombState bomb_state_ {BombState::placed};
	static constexpr au::QuantityF<au::Seconds> bomb_detonation_counter_ {au::seconds(3.F)};

	static constexpr eclipse::Map<BombPlacedAnimation, SubTextureIndex, 3> bomb_sprite_placed_ {
	    {{{BombPlacedAnimation::big, {.x = pixels(0), .y = pixels(19)}},
	      {BombPlacedAnimation::medium, {.x = pixels(1), .y = pixels(19)}},
	      {BombPlacedAnimation::small, {.x = pixels(2), .y = pixels(19)}}}}};

	static constexpr eclipse::Map<BombExplodedAnimation, SubTextureIndex, 4> bomb_sprite_exploded_ {
	    {{{BombExplodedAnimation::small, {.x = pixels(2), .y = pixels(16)}},
	      {BombExplodedAnimation::medium, {.x = pixels(7), .y = pixels(16)}},
	      {BombExplodedAnimation::big, {.x = pixels(2), .y = pixels(11)}},
	      {BombExplodedAnimation::biggest, {.x = pixels(7), .y = pixels(11)}}}}};

	static constexpr std::array<BombPlacedAnimation, 4> bomb_placed_animation_sequence_ {
	    BombPlacedAnimation::big, BombPlacedAnimation::medium, BombPlacedAnimation::small, BombPlacedAnimation::medium};

	static constexpr std::array<BombExplodedAnimation, 7> bomb_exploded_animation_sequence_ {
	    BombExplodedAnimation::small,   BombExplodedAnimation::medium, BombExplodedAnimation::big,
	    BombExplodedAnimation::biggest, BombExplodedAnimation::big,    BombExplodedAnimation::medium,
	    BombExplodedAnimation::small};
};

}  // namespace bomberman