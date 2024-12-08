#pragma once

#include <Eclipse.h>
#include <string>
#include "common_types.hpp"

namespace bomberman {

enum class EnemyDirection { left, right };
enum class EnemyState { alive, dead, expired };
enum class EnemySpriteAnimation { first, second, third };
enum class EnemySpriteDeadAnimation { first, second, third, forth, fifth };

static constexpr std::array<EnemySpriteAnimation, 4> SPRITE_ALIVE_ANIMATION_SEQUENCE = {
    EnemySpriteAnimation::first, EnemySpriteAnimation::second, EnemySpriteAnimation::third,
    EnemySpriteAnimation::second};
static constexpr std::array<EnemySpriteDeadAnimation, 9> SPRITE_DEAD_ANIMATION_SEQUENCE = {
    EnemySpriteDeadAnimation::first, EnemySpriteDeadAnimation::first, EnemySpriteDeadAnimation::first,
    EnemySpriteDeadAnimation::first, EnemySpriteDeadAnimation::first, EnemySpriteDeadAnimation::second,
    EnemySpriteDeadAnimation::third, EnemySpriteDeadAnimation::forth, EnemySpriteDeadAnimation::fifth};

struct EnemySprite {
	EnemyDirection direction {EnemyDirection::left};
	EnemySpriteAnimation animation {EnemySpriteAnimation::first};

	void flip_direction() {
		if (direction == EnemyDirection::left) {
			direction = EnemyDirection::right;
		} else if (direction == EnemyDirection::right) {
			direction = EnemyDirection::left;
		}
	}

	auto operator<=>(const EnemySprite&) const = default;
};

using eclipse::units::pixels;

class Enemy : public EntityInterface {
public:
	explicit Enemy(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	               const eclipse::ref<eclipse::Scene>& context);

	~Enemy() override { destroy(); };

	void on_update(au::QuantityF<au::Seconds> timestep) override;

	void destroy() override;

	void on_bomb_ray_hit() override;

	CellContent on_player_interaction() override { return CellContent::enemy; }

	[[nodiscard]] bool has_expired() const override { return enemy_state_ == EnemyState::expired; }

	[[nodiscard]] virtual const eclipse::Point2D& get_position() const override { return position_; };

protected:
	void create_entity(const std::string& entity_name, const SubTextureIndex& index);

	eclipse::Point2D position_;
	eclipse::ref<eclipse::TextureSheet> texture_sheet_;
	eclipse::ref<eclipse::Scene> context_;
	eclipse::Entity entity_;
	eclipse::ref<eclipse::SubTexture2D> texture_enemy_;
	EnemySprite sprite_alive_;
	EnemyState enemy_state_ {EnemyState::alive};
	EnemySpriteDeadAnimation sprite_dead_ {EnemySpriteDeadAnimation::first};
	au::QuantityF<au::Seconds> blink_timer_     = au::seconds(0.F);
	uint32_t sprite_animation_sequence_counter_ = 0;
};

class EnemyBalloon : public Enemy {
public:
	explicit EnemyBalloon(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                      const eclipse::ref<eclipse::Scene>& context)
	    : Enemy(texture_sheet, position, context) {
		create_entity("Balloon enemy", enemy_texture_index_.at(sprite_alive_));
	}

protected:
	void update_texture() override {
		if (enemy_state_ == EnemyState::alive) {
			const auto& sub_texture_index = enemy_texture_index_.at(sprite_alive_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		} else if (enemy_state_ == EnemyState::dead) {
			const auto& sub_texture_index = enemy_texture_index_dead_.at(sprite_dead_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		}
	}

private:
	static constexpr eclipse::Map<EnemySprite, SubTextureIndex, 6> enemy_texture_index_ {
	    {{{{EnemyDirection::right, EnemySpriteAnimation::first}, {.x = pixels(0), .y = pixels(7)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::second}, {.x = pixels(1), .y = pixels(7)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::third}, {.x = pixels(2), .y = pixels(7)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::first}, {.x = pixels(3), .y = pixels(7)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::second}, {.x = pixels(4), .y = pixels(7)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::third}, {.x = pixels(5), .y = pixels(7)}}}}};

	static constexpr eclipse::Map<EnemySpriteDeadAnimation, SubTextureIndex, 5> enemy_texture_index_dead_ {
	    {{{EnemySpriteDeadAnimation::first, {.x = pixels(6), .y = pixels(7)}},
	      {EnemySpriteDeadAnimation::second, {.x = pixels(7), .y = pixels(7)}},
	      {EnemySpriteDeadAnimation::third, {.x = pixels(8), .y = pixels(7)}},
	      {EnemySpriteDeadAnimation::forth, {.x = pixels(9), .y = pixels(7)}},
	      {EnemySpriteDeadAnimation::fifth, {.x = pixels(10), .y = pixels(7)}}}}};
};

class EnemyDrop : public Enemy {
public:
	explicit EnemyDrop(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                   const eclipse::ref<eclipse::Scene>& context)
	    : Enemy(texture_sheet, position, context) {
		create_entity("Drop enemy", enemy_texture_index_.at(sprite_alive_));
	}

protected:
	void update_texture() override {
		if (enemy_state_ == EnemyState::alive) {
			const auto& sub_texture_index = enemy_texture_index_.at(sprite_alive_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		} else if (enemy_state_ == EnemyState::dead) {
			const auto& sub_texture_index = enemy_texture_index_dead_.at(sprite_dead_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		}
	}

private:
	static constexpr eclipse::Map<EnemySprite, SubTextureIndex, 6> enemy_texture_index_ {
	    {{{{EnemyDirection::right, EnemySpriteAnimation::first}, {.x = pixels(0), .y = pixels(6)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::second}, {.x = pixels(1), .y = pixels(6)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::third}, {.x = pixels(2), .y = pixels(6)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::first}, {.x = pixels(3), .y = pixels(6)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::second}, {.x = pixels(4), .y = pixels(6)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::third}, {.x = pixels(5), .y = pixels(6)}}}}};

	static constexpr eclipse::Map<EnemySpriteDeadAnimation, SubTextureIndex, 5> enemy_texture_index_dead_ {
	    {{{EnemySpriteDeadAnimation::first, {.x = pixels(6), .y = pixels(6)}},
	      {EnemySpriteDeadAnimation::second, {.x = pixels(7), .y = pixels(4)}},
	      {EnemySpriteDeadAnimation::third, {.x = pixels(8), .y = pixels(4)}},
	      {EnemySpriteDeadAnimation::forth, {.x = pixels(9), .y = pixels(4)}},
	      {EnemySpriteDeadAnimation::fifth, {.x = pixels(10), .y = pixels(4)}}}}};
};

class EnemyStriped : public Enemy {
public:
	explicit EnemyStriped(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                      const eclipse::ref<eclipse::Scene>& context)
	    : Enemy(texture_sheet, position, context) {
		create_entity("Striped enemy", enemy_texture_index_.at(sprite_alive_));
	}

protected:
	void update_texture() override {
		if (enemy_state_ == EnemyState::alive) {
			const auto& sub_texture_index = enemy_texture_index_.at(sprite_alive_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		} else if (enemy_state_ == EnemyState::dead) {
			const auto& sub_texture_index = enemy_texture_index_dead_.at(sprite_dead_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		}
	}

private:
	static constexpr eclipse::Map<EnemySprite, SubTextureIndex, 6> enemy_texture_index_ {
	    {{{{EnemyDirection::right, EnemySpriteAnimation::first}, {.x = pixels(0), .y = pixels(5)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::second}, {.x = pixels(1), .y = pixels(5)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::third}, {.x = pixels(2), .y = pixels(5)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::first}, {.x = pixels(3), .y = pixels(5)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::second}, {.x = pixels(4), .y = pixels(5)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::third}, {.x = pixels(5), .y = pixels(5)}}}}};

	static constexpr eclipse::Map<EnemySpriteDeadAnimation, SubTextureIndex, 5> enemy_texture_index_dead_ {
	    {{{EnemySpriteDeadAnimation::first, {.x = pixels(6), .y = pixels(5)}},
	      {EnemySpriteDeadAnimation::second, {.x = pixels(7), .y = pixels(5)}},
	      {EnemySpriteDeadAnimation::third, {.x = pixels(8), .y = pixels(5)}},
	      {EnemySpriteDeadAnimation::forth, {.x = pixels(9), .y = pixels(5)}},
	      {EnemySpriteDeadAnimation::fifth, {.x = pixels(10), .y = pixels(5)}}}}};
};

class EnemyRound : public Enemy {
public:
	explicit EnemyRound(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                    const eclipse::ref<eclipse::Scene>& context)
	    : Enemy(texture_sheet, position, context) {
		create_entity("Round enemy", enemy_texture_index_.at(sprite_alive_));
	}

protected:
	void update_texture() override {
		if (enemy_state_ == EnemyState::alive) {
			const auto& sub_texture_index = enemy_texture_index_.at(sprite_alive_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		} else if (enemy_state_ == EnemyState::dead) {
			const auto& sub_texture_index = enemy_texture_index_dead_.at(sprite_dead_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		}
	}

private:
	static constexpr eclipse::Map<EnemySprite, SubTextureIndex, 6> enemy_texture_index_ {
	    {{{{EnemyDirection::right, EnemySpriteAnimation::first}, {.x = pixels(0), .y = pixels(4)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::second}, {.x = pixels(1), .y = pixels(4)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::third}, {.x = pixels(2), .y = pixels(4)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::first}, {.x = pixels(3), .y = pixels(4)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::second}, {.x = pixels(4), .y = pixels(4)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::third}, {.x = pixels(5), .y = pixels(4)}}}}};

	static constexpr eclipse::Map<EnemySpriteDeadAnimation, SubTextureIndex, 5> enemy_texture_index_dead_ {
	    {{{EnemySpriteDeadAnimation::first, {.x = pixels(6), .y = pixels(4)}},
	      {EnemySpriteDeadAnimation::second, {.x = pixels(7), .y = pixels(7)}},
	      {EnemySpriteDeadAnimation::third, {.x = pixels(8), .y = pixels(7)}},
	      {EnemySpriteDeadAnimation::forth, {.x = pixels(9), .y = pixels(7)}},
	      {EnemySpriteDeadAnimation::fifth, {.x = pixels(10), .y = pixels(7)}}}}};
};

class EnemyWater : public Enemy {
public:
	explicit EnemyWater(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                    const eclipse::ref<eclipse::Scene>& context)
	    : Enemy(texture_sheet, position, context) {
		create_entity("Water enemy", enemy_texture_index_.at(sprite_alive_));
	}

protected:
	void update_texture() override {
		if (enemy_state_ == EnemyState::alive) {
			const auto& sub_texture_index = enemy_texture_index_.at(sprite_alive_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		} else if (enemy_state_ == EnemyState::dead) {
			const auto& sub_texture_index = enemy_texture_index_dead_.at(sprite_dead_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		}
	}

private:
	static constexpr eclipse::Map<EnemySprite, SubTextureIndex, 6> enemy_texture_index_ {
	    {{{{EnemyDirection::right, EnemySpriteAnimation::first}, {.x = pixels(0), .y = pixels(3)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::second}, {.x = pixels(1), .y = pixels(3)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::third}, {.x = pixels(2), .y = pixels(3)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::first}, {.x = pixels(3), .y = pixels(3)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::second}, {.x = pixels(4), .y = pixels(3)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::third}, {.x = pixels(5), .y = pixels(3)}}}}};

	static constexpr eclipse::Map<EnemySpriteDeadAnimation, SubTextureIndex, 5> enemy_texture_index_dead_ {
	    {{{EnemySpriteDeadAnimation::first, {.x = pixels(6), .y = pixels(3)}},
	      {EnemySpriteDeadAnimation::second, {.x = pixels(7), .y = pixels(4)}},
	      {EnemySpriteDeadAnimation::third, {.x = pixels(8), .y = pixels(4)}},
	      {EnemySpriteDeadAnimation::forth, {.x = pixels(9), .y = pixels(4)}},
	      {EnemySpriteDeadAnimation::fifth, {.x = pixels(10), .y = pixels(4)}}}}};
};

class EnemyGhost : public Enemy {
public:
	explicit EnemyGhost(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                    const eclipse::ref<eclipse::Scene>& context)
	    : Enemy(texture_sheet, position, context) {
		create_entity("Ghost enemy", enemy_texture_index_.at(sprite_alive_));
	}

protected:
	void update_texture() override {
		if (enemy_state_ == EnemyState::alive) {
			const auto& sub_texture_index = enemy_texture_index_.at(sprite_alive_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		} else if (enemy_state_ == EnemyState::dead) {
			const auto& sub_texture_index = enemy_texture_index_dead_.at(sprite_dead_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		}
	}

private:
	static constexpr eclipse::Map<EnemySprite, SubTextureIndex, 6> enemy_texture_index_ {
	    {{{{EnemyDirection::right, EnemySpriteAnimation::first}, {.x = pixels(0), .y = pixels(2)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::second}, {.x = pixels(1), .y = pixels(2)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::third}, {.x = pixels(2), .y = pixels(2)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::first}, {.x = pixels(3), .y = pixels(2)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::second}, {.x = pixels(4), .y = pixels(2)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::third}, {.x = pixels(5), .y = pixels(2)}}}}};

	static constexpr eclipse::Map<EnemySpriteDeadAnimation, SubTextureIndex, 5> enemy_texture_index_dead_ {
	    {{{EnemySpriteDeadAnimation::first, {.x = pixels(6), .y = pixels(2)}},
	      {EnemySpriteDeadAnimation::second, {.x = pixels(7), .y = pixels(5)}},
	      {EnemySpriteDeadAnimation::third, {.x = pixels(8), .y = pixels(5)}},
	      {EnemySpriteDeadAnimation::forth, {.x = pixels(9), .y = pixels(5)}},
	      {EnemySpriteDeadAnimation::fifth, {.x = pixels(10), .y = pixels(5)}}}}};
};

class EnemyBear : public Enemy {
public:
	explicit EnemyBear(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                   const eclipse::ref<eclipse::Scene>& context)
	    : Enemy(texture_sheet, position, context) {
		create_entity("Bear enemy", enemy_texture_index_.at(sprite_alive_));
	}

protected:
	void update_texture() override {
		if (enemy_state_ == EnemyState::alive) {
			const auto& sub_texture_index = enemy_texture_index_.at(sprite_alive_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		} else if (enemy_state_ == EnemyState::dead) {
			const auto& sub_texture_index = enemy_texture_index_dead_.at(sprite_dead_);
			texture_enemy_->set_index_x(sub_texture_index.x);
			texture_enemy_->set_index_y(sub_texture_index.y);
		}
	}

private:
	static constexpr eclipse::Map<EnemySprite, SubTextureIndex, 6> enemy_texture_index_ {
	    {{{{EnemyDirection::right, EnemySpriteAnimation::first}, {.x = pixels(0), .y = pixels(1)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::second}, {.x = pixels(1), .y = pixels(1)}},
	      {{EnemyDirection::right, EnemySpriteAnimation::third}, {.x = pixels(2), .y = pixels(1)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::first}, {.x = pixels(3), .y = pixels(1)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::second}, {.x = pixels(4), .y = pixels(1)}},
	      {{EnemyDirection::left, EnemySpriteAnimation::third}, {.x = pixels(5), .y = pixels(1)}}}}};

	static constexpr eclipse::Map<EnemySpriteDeadAnimation, SubTextureIndex, 5> enemy_texture_index_dead_ {
	    {{{EnemySpriteDeadAnimation::first, {.x = pixels(6), .y = pixels(1)}},
	      {EnemySpriteDeadAnimation::second, {.x = pixels(7), .y = pixels(7)}},
	      {EnemySpriteDeadAnimation::third, {.x = pixels(8), .y = pixels(7)}},
	      {EnemySpriteDeadAnimation::forth, {.x = pixels(9), .y = pixels(7)}},
	      {EnemySpriteDeadAnimation::fifth, {.x = pixels(10), .y = pixels(7)}}}}};
};

}  // namespace bomberman