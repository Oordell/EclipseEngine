#pragma once

#include <Eclipse.h>
#include "common_types.hpp"

namespace bomberman {

enum class BombRayAnimation { small, medium, big, biggest };
enum class BombRaySection { midt, end };
enum class BombRayDirection { left, right, up, down };
enum class BombRayExpired { no, yes };

struct BombRayDescription {
	BombRayAnimation size {BombRayAnimation::small};
	BombRaySection section {BombRaySection::midt};
	BombRayDirection direction {BombRayDirection::left};

	auto operator<=>(const BombRayDescription&) const = default;
};

using eclipse::units::pixels;

class BombRay : public EntityInterface {
public:
	explicit BombRay(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                 const eclipse::ref<eclipse::Scene>& context, const BombRayDescription& initial_state)
	    : texture_sheet_(texture_sheet), position_(position), context_(context), bomb_ray_state_(initial_state) {
		create_entity();
	}

	void on_update(au::QuantityF<au::Seconds> timestep) override;
	void destroy() override;

	void on_bomb_ray_hit() override {}

	CellContent on_player_interaction() override { return CellContent::bomb_ray; };

	[[nodiscard]] bool has_expired() const override { return expired_ == BombRayExpired::yes; }

	[[nodiscard]] const eclipse::Point2D& get_position() const override { return position_; };

protected:
	void update_texture() override;

private:
	void create_entity(const std::string& entity_name = "Bomb Ray");

	eclipse::Point2D position_;
	eclipse::ref<eclipse::TextureSheet> texture_sheet_;
	eclipse::ref<eclipse::Scene> context_;
	SubTextureIndex texture_sheet_coordinates_;
	eclipse::Entity entity_;
	eclipse::ref<eclipse::SubTexture2D> texture_bomb_ray_;

	au::QuantityF<au::Seconds> blink_timer_     = au::seconds(0.F);
	uint32_t sprite_animation_sequence_counter_ = 0;
	BombRayDescription bomb_ray_state_;
	BombRayExpired expired_ {BombRayExpired::no};

	static constexpr eclipse::Map<BombRayDescription, SubTextureIndex, 32> bomb_ray_sprites_ {
	    {{{{BombRayAnimation::small, BombRaySection::midt, BombRayDirection::left}, {pixels(23), pixels(17)}},
	      {{BombRayAnimation::small, BombRaySection::midt, BombRayDirection::right}, {pixels(23), pixels(17)}},
	      {{BombRayAnimation::small, BombRaySection::midt, BombRayDirection::up}, {pixels(22), pixels(12)}},
	      {{BombRayAnimation::small, BombRaySection::midt, BombRayDirection::down}, {pixels(22), pixels(12)}},
	      {{BombRayAnimation::small, BombRaySection::end, BombRayDirection::left}, {pixels(22), pixels(17)}},
	      {{BombRayAnimation::small, BombRaySection::end, BombRayDirection::right}, {pixels(24), pixels(17)}},
	      {{BombRayAnimation::small, BombRaySection::end, BombRayDirection::up}, {pixels(22), pixels(13)}},
	      {{BombRayAnimation::small, BombRaySection::end, BombRayDirection::down}, {pixels(22), pixels(11)}},

	      {{BombRayAnimation::medium, BombRaySection::midt, BombRayDirection::left}, {pixels(23), pixels(16)}},
	      {{BombRayAnimation::medium, BombRaySection::midt, BombRayDirection::right}, {pixels(23), pixels(16)}},
	      {{BombRayAnimation::medium, BombRaySection::midt, BombRayDirection::up}, {pixels(23), pixels(12)}},
	      {{BombRayAnimation::medium, BombRaySection::midt, BombRayDirection::down}, {pixels(23), pixels(12)}},
	      {{BombRayAnimation::medium, BombRaySection::end, BombRayDirection::left}, {pixels(22), pixels(16)}},
	      {{BombRayAnimation::medium, BombRaySection::end, BombRayDirection::right}, {pixels(24), pixels(16)}},
	      {{BombRayAnimation::medium, BombRaySection::end, BombRayDirection::up}, {pixels(23), pixels(13)}},
	      {{BombRayAnimation::medium, BombRaySection::end, BombRayDirection::down}, {pixels(23), pixels(11)}},

	      {{BombRayAnimation::big, BombRaySection::midt, BombRayDirection::left}, {pixels(23), pixels(15)}},
	      {{BombRayAnimation::big, BombRaySection::midt, BombRayDirection::right}, {pixels(23), pixels(15)}},
	      {{BombRayAnimation::big, BombRaySection::midt, BombRayDirection::up}, {pixels(24), pixels(12)}},
	      {{BombRayAnimation::big, BombRaySection::midt, BombRayDirection::down}, {pixels(24), pixels(12)}},
	      {{BombRayAnimation::big, BombRaySection::end, BombRayDirection::left}, {pixels(22), pixels(15)}},
	      {{BombRayAnimation::big, BombRaySection::end, BombRayDirection::right}, {pixels(24), pixels(15)}},
	      {{BombRayAnimation::big, BombRaySection::end, BombRayDirection::up}, {pixels(24), pixels(13)}},
	      {{BombRayAnimation::big, BombRaySection::end, BombRayDirection::down}, {pixels(24), pixels(11)}},

	      {{BombRayAnimation::biggest, BombRaySection::midt, BombRayDirection::left}, {pixels(23), pixels(14)}},
	      {{BombRayAnimation::biggest, BombRaySection::midt, BombRayDirection::right}, {pixels(23), pixels(14)}},
	      {{BombRayAnimation::biggest, BombRaySection::midt, BombRayDirection::up}, {pixels(25), pixels(12)}},
	      {{BombRayAnimation::biggest, BombRaySection::midt, BombRayDirection::down}, {pixels(25), pixels(12)}},
	      {{BombRayAnimation::biggest, BombRaySection::end, BombRayDirection::left}, {pixels(22), pixels(14)}},
	      {{BombRayAnimation::biggest, BombRaySection::end, BombRayDirection::right}, {pixels(24), pixels(14)}},
	      {{BombRayAnimation::biggest, BombRaySection::end, BombRayDirection::up}, {pixels(25), pixels(13)}},
	      {{BombRayAnimation::biggest, BombRaySection::end, BombRayDirection::down}, {pixels(25), pixels(11)}}}}};

	static constexpr std::array<BombRayAnimation, 7> bomb_ray_animation_sequence_ {
	    BombRayAnimation::small, BombRayAnimation::medium, BombRayAnimation::big,  BombRayAnimation::biggest,
	    BombRayAnimation::big,   BombRayAnimation::medium, BombRayAnimation::small};
};

}  // namespace bomberman