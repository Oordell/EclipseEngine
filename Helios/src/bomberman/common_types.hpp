#pragma once

#include <au.hh>
#include <Eclipse.h>

namespace bomberman {

namespace details {
static constexpr float Z_GRASS       = 0.0F;
static constexpr float Z_SOLID_WALLS = 0.1F;
static constexpr float Z_PLAYER      = 0.2F;
static constexpr float Z_ITEMS       = 0.1F;
static constexpr float Z_ENEMIES     = 0.2F;
static constexpr float Z_BOMB        = 0.1F;
static constexpr float Z_BOMB_RAY    = 0.1F;
static constexpr float Z_SOFT_WALS   = 0.2F;
}  // namespace details

struct SubTextureIndex {
	au::Quantity<eclipse::units::Pixels, uint32_t> x;
	au::Quantity<eclipse::units::Pixels, uint32_t> y;
};

enum class CellContent {
	frame_corner_top_left,
	frame_corner_top_right,
	frame_corner_buttom_left,
	frame_corner_buttom_right,
	frame_left_1,
	frame_left_2,
	frame_left_3,
	frame_right_1,
	frame_right_2,
	frame_right_3,
	frame_top_1,
	frame_top_2,
	frame_top_3,
	frame_bottom_1,
	frame_bottom_2,
	frame_bottom_3,
	wall_solid,
	wall_soft,
	solid_wall,
	grass,
	soft_wall,
	start_pose,
	item_bomb_increment,
	item_bomb_reach,
	item_speed_boost,
	item_invincibility,
	enemy,
	enemy_balloon,
	enemy_drop,
	enemy_striped,
	enemy_round,
	enemy_water,
	enemy_ghost,
	enemy_bear,
	enemy_coin,
	bomb,
	bomb_ray,
};

class EntityInterface {
public:
	virtual ~EntityInterface()                                         = default;
	virtual void on_update(au::QuantityF<au::Seconds> timestep)        = 0;
	virtual void destroy()                                             = 0;
	virtual void on_bomb_ray_hit()                                     = 0;
	virtual CellContent on_player_interaction()                        = 0;
	[[nodiscard]] virtual bool has_expired() const                     = 0;
	[[nodiscard]] virtual const eclipse::Point2D& get_position() const = 0;

protected:
	virtual void update_texture() = 0;
};

class EnemyInterface {
public:
	virtual void on_update(au::QuantityF<au::Seconds> timestep) = 0;
	[[nodiscard]] virtual bool enemy_has_expired() const        = 0;
	virtual void destroy()                                      = 0;

private:
	virtual void update_enemy_texture() = 0;
};

}  // namespace bomberman