#pragma once

#include <Eclipse.h>
#include <string>
#include <type_traits>
#include <array>
#include <stdexcept>
#include <variant>
#include "common_types.hpp"
#include "enemy_data.hpp"
#include "playing_board.hpp"
#include "movement_interface.hpp"

namespace bomberman {

using eclipse::units::pixels;

class Enemy : public EntityInterface {
public:
	explicit Enemy(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	               const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board,
	               const EnemyMetaData& enemy_data);

	~Enemy() override { destroy(); };

	void on_update(au::QuantityF<au::Seconds> timestep) override;

	void destroy() override;

	void on_bomb_ray_hit() override;

	[[nodiscard]] CellContent on_player_interaction() override { return CellContent::enemy; }

	[[nodiscard]] bool has_expired() const override { return enemy_state_ == EnemyState::expired; }

	[[nodiscard]] bool is_dying() const { return enemy_state_ == EnemyState::dead; }

	[[nodiscard]] const eclipse::Point2D& get_position() const override { return position_; };

	[[nodiscard]] const EnemyMetaData& get_enemy_meta_data() const { return enemy_data_; }

	void set_enemy_direction(EnemyDirection new_direction) { sprite_alive_.direction = new_direction; }

	void set_enemy_position(const eclipse::Point2D& new_position) {
		position_                                                            = new_position;
		entity_.get_component<eclipse::component::Transform>().translation.x = position_.x.in<float>(pixels);
		entity_.get_component<eclipse::component::Transform>().translation.y = position_.y.in<float>(pixels);
	}

protected:
	void create_entity(const std::string& entity_name = "Enemy");

	void update_texture() override;

	static constexpr eclipse::Map<EnemySprite, SubTextureIndex, 16> enemy_texture_index_alive_ {
	    {{{{EnemyDirection::down, EnemySpriteAliveAnimation::first}, {.x = pixels(0), .y = pixels(0)}},
	      {{EnemyDirection::down, EnemySpriteAliveAnimation::second}, {.x = pixels(1), .y = pixels(0)}},
	      {{EnemyDirection::down, EnemySpriteAliveAnimation::third}, {.x = pixels(2), .y = pixels(0)}},
	      {{EnemyDirection::down, EnemySpriteAliveAnimation::forth}, {.x = pixels(3), .y = pixels(0)}},
	      {{EnemyDirection::left, EnemySpriteAliveAnimation::first}, {.x = pixels(4), .y = pixels(0)}},
	      {{EnemyDirection::left, EnemySpriteAliveAnimation::second}, {.x = pixels(5), .y = pixels(0)}},
	      {{EnemyDirection::left, EnemySpriteAliveAnimation::third}, {.x = pixels(6), .y = pixels(0)}},
	      {{EnemyDirection::left, EnemySpriteAliveAnimation::forth}, {.x = pixels(7), .y = pixels(0)}},
	      {{EnemyDirection::up, EnemySpriteAliveAnimation::first}, {.x = pixels(8), .y = pixels(0)}},
	      {{EnemyDirection::up, EnemySpriteAliveAnimation::second}, {.x = pixels(9), .y = pixels(0)}},
	      {{EnemyDirection::up, EnemySpriteAliveAnimation::third}, {.x = pixels(10), .y = pixels(0)}},
	      {{EnemyDirection::up, EnemySpriteAliveAnimation::forth}, {.x = pixels(11), .y = pixels(0)}},
	      {{EnemyDirection::right, EnemySpriteAliveAnimation::first}, {.x = pixels(12), .y = pixels(0)}},
	      {{EnemyDirection::right, EnemySpriteAliveAnimation::second}, {.x = pixels(13), .y = pixels(0)}},
	      {{EnemyDirection::right, EnemySpriteAliveAnimation::third}, {.x = pixels(14), .y = pixels(0)}},
	      {{EnemyDirection::right, EnemySpriteAliveAnimation::forth}, {.x = pixels(15), .y = pixels(0)}}}}};

	static constexpr eclipse::Map<EnemySpriteDeadAnimation, SubTextureIndex, 6> enemy_texture_index_dead_ {
	    {{{EnemySpriteDeadAnimation::first, {.x = pixels(16), .y = pixels(0)}},
	      {EnemySpriteDeadAnimation::second, {.x = pixels(17), .y = pixels(0)}},
	      {EnemySpriteDeadAnimation::third, {.x = pixels(18), .y = pixels(0)}},
	      {EnemySpriteDeadAnimation::forth, {.x = pixels(19), .y = pixels(0)}},
	      {EnemySpriteDeadAnimation::fifth, {.x = pixels(20), .y = pixels(0)}},
	      {EnemySpriteDeadAnimation::sixth, {.x = pixels(21), .y = pixels(0)}}}}};

	static constexpr std::array<EnemySpriteAliveAnimation, 4> SPRITE_ALIVE_ANIMATION_SEQUENCE = {
	    EnemySpriteAliveAnimation::first, EnemySpriteAliveAnimation::second, EnemySpriteAliveAnimation::third,
	    EnemySpriteAliveAnimation::forth};
	static constexpr std::array<EnemySpriteDeadAnimation, 10> SPRITE_DEAD_ANIMATION_SEQUENCE = {
	    EnemySpriteDeadAnimation::first, EnemySpriteDeadAnimation::first, EnemySpriteDeadAnimation::first,
	    EnemySpriteDeadAnimation::first, EnemySpriteDeadAnimation::first, EnemySpriteDeadAnimation::second,
	    EnemySpriteDeadAnimation::third, EnemySpriteDeadAnimation::forth, EnemySpriteDeadAnimation::fifth,
	    EnemySpriteDeadAnimation::sixth};

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
	EnemyMetaData enemy_data_;
	eclipse::scope<MovementInterface> movement_;
};

class EnemyRedBalloon : public Enemy {
public:
	explicit EnemyRedBalloon(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                         const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_red_balloon,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_red_balloon");
	}
};

class EnemyGreenDuck : public Enemy {
public:
	explicit EnemyGreenDuck(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                        const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_green_duck,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 2.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_green_duck");
	}
};

class EnemyBlueFrog : public Enemy {
public:
	explicit EnemyBlueFrog(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                       const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_blue_frog,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_blue_frog");
	}
};

class EnemyBlueBat : public Enemy {
public:
	explicit EnemyBlueBat(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                      const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_blue_bat,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 3.F,
	             .ignore_soft_walls = true}) {
		create_entity("enemy_blue_bat");
	}
};

class EnemyWhiteGhost : public Enemy {
public:
	explicit EnemyWhiteGhost(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                         const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_white_ghost,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_white_ghost");
	}
};

class EnemyRedFlower : public Enemy {
public:
	explicit EnemyRedFlower(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                        const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_red_flower,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_red_flower");
	}
};

class EnemyRedBear : public Enemy {
public:
	explicit EnemyRedBear(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                      const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_red_bear,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_red_bear");
	}
};

class EnemyRedCoin : public Enemy {
public:
	explicit EnemyRedCoin(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                      const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_red_coin,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_red_coin");
	}
};

class EnemyGreenFish : public Enemy {
public:
	explicit EnemyGreenFish(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                        const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_green_fish,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_green_fish");
	}
};

class EnemyBlueCloud : public Enemy {
public:
	explicit EnemyBlueCloud(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                        const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_blue_cloud,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_blue_cloud");
	}
};

class EnemyBlueFog : public Enemy {
public:
	explicit EnemyBlueFog(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                      const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_blue_fog,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_blue_fog");
	}
};

class EnemyGreenSmileyBouncer : public Enemy {
public:
	explicit EnemyGreenSmileyBouncer(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                                 const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_green_smiley_bouncer,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_green_smiley_bouncer");
	}
};

class EnemyGreenCroc : public Enemy {
public:
	explicit EnemyGreenCroc(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                        const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_green_croc,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_green_croc");
	}
};

class EnemyBlueJellyfish : public Enemy {
public:
	explicit EnemyBlueJellyfish(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                            const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_blue_jellyfish,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_blue_jellyfish");
	}
};

class EnemyBlueSpinningTop : public Enemy {
public:
	explicit EnemyBlueSpinningTop(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                              const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_blue_spinning_top,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_blue_spinning_top");
	}
};

class EnemyGreenFrog : public Enemy {
public:
	explicit EnemyGreenFrog(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                        const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_green_frog,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_green_frog");
	}
};

class EnemyGreenBomb : public Enemy {
public:
	explicit EnemyGreenBomb(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                        const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_green_bomb,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_green_bomb");
	}
};

class EnemyGreenWhiteCentipede : public Enemy {
public:
	explicit EnemyGreenWhiteCentipede(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                                  const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_green_white_centipede,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_green_white_centipede");
	}
};

class EnemyGreenBoldRoller : public Enemy {
public:
	explicit EnemyGreenBoldRoller(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                              const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board)
	    : Enemy(texture_sheet, position, context, playing_board,
	            {.enemy_type        = EnemyType::enemy_green_bold_roller,
	             .movement_type     = EnemyMovementType::random,
	             .speed             = 1.F,
	             .ignore_soft_walls = false}) {
		create_entity("enemy_green_bold_roller");
	}
};

}  // namespace bomberman