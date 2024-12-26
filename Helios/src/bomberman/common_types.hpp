#pragma once

#include <au.hh>
#include <Eclipse.h>

namespace bomberman {

namespace details {
static constexpr float Z_GRASS                           = 0.0F;
static constexpr float Z_SOLID_WALLS                     = 0.1F;
static constexpr float Z_PLAYER                          = 0.4F;
static constexpr float Z_ITEMS                           = 0.1F;
static constexpr float Z_ENEMIES                         = 0.3F;
static constexpr float Z_BOMB                            = 0.1F;
static constexpr float Z_BOMB_RAY                        = 0.1F;
static constexpr float Z_SOFT_WALS                       = 0.2F;
static constexpr uint32_t texture_sheet_level_separator_ = 5;
}  // namespace details

struct SubTextureIndex {
	au::Quantity<eclipse::units::Pixels, uint32_t> x;
	au::Quantity<eclipse::units::Pixels, uint32_t> y;
};

enum class CellContent {
	enemy_green_bold_roller     = 0,
	enemy_green_white_centipede = 1,
	enemy_green_bomb            = 2,
	enemy_green_frog            = 3,
	enemy_blue_spinning_top     = 4,
	enemy_blue_jellyfish        = 5,
	enemy_green_croc            = 6,
	enemy_green_smiley_bouncer  = 7,
	enemy_blue_fog              = 8,
	enemy_blue_cloud            = 9,
	enemy_green_fish            = 10,
	enemy_red_coin              = 11,
	enemy_red_bear              = 12,
	enemy_red_flower            = 13,
	enemy_white_ghost           = 14,
	enemy_blue_bat              = 15,
	enemy_blue_frog             = 16,
	enemy_green_duck            = 17,
	enemy_red_balloon           = 18,
	enemy,
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
	grass,
	start_pose,
	item_bomb_increment,
	item_bomb_reach,
	item_speed_boost,
	item_invincibility,
	bomb,
	bomb_ray,
};

enum class EnemyType {
	enemy_red_balloon           = CellContent::enemy_red_balloon,
	enemy_green_duck            = CellContent::enemy_green_duck,
	enemy_blue_frog             = CellContent::enemy_blue_frog,
	enemy_blue_bat              = CellContent::enemy_blue_bat,
	enemy_white_ghost           = CellContent::enemy_white_ghost,
	enemy_red_flower            = CellContent::enemy_red_flower,
	enemy_red_bear              = CellContent::enemy_red_bear,
	enemy_red_coin              = CellContent::enemy_red_coin,
	enemy_green_fish            = CellContent::enemy_green_fish,
	enemy_blue_cloud            = CellContent::enemy_blue_cloud,
	enemy_blue_fog              = CellContent::enemy_blue_fog,
	enemy_green_smiley_bouncer  = CellContent::enemy_green_smiley_bouncer,
	enemy_green_croc            = CellContent::enemy_green_croc,
	enemy_blue_jellyfish        = CellContent::enemy_blue_jellyfish,
	enemy_blue_spinning_top     = CellContent::enemy_blue_spinning_top,
	enemy_green_frog            = CellContent::enemy_green_frog,
	enemy_green_bomb            = CellContent::enemy_green_bomb,
	enemy_green_white_centipede = CellContent::enemy_green_white_centipede,
	enemy_green_bold_roller     = CellContent::enemy_green_bold_roller,
};

class EntityInterface {
public:
	virtual ~EntityInterface()                                         = default;
	virtual void on_update(au::QuantityF<au::Seconds> timestep)        = 0;
	virtual void destroy()                                             = 0;
	virtual void on_bomb_ray_hit()                                     = 0;
	[[nodiscard]] virtual CellContent on_player_interaction()          = 0;
	[[nodiscard]] virtual bool has_expired() const                     = 0;
	[[nodiscard]] virtual const eclipse::Point2D& get_position() const = 0;

protected:
	virtual void update_texture() = 0;
};

}  // namespace bomberman