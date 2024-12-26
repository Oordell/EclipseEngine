#pragma once

#include <Eclipse.h>
#include <vector>
#include "common_types.hpp"

namespace bomberman {

namespace details {
static constexpr uint32_t NUM_OF_DIFFERENT_FRAME_ELEMENTS = 3;
}

static constexpr eclipse::Map<char, CellContent, 27> char_to_cell_content = {
    {{{'*', CellContent::wall_solid},
      {'_', CellContent::grass},
      {'&', CellContent::start_pose},
      {'#', CellContent::wall_soft},
      {'a', CellContent::enemy_red_balloon},
      {'b', CellContent::enemy_green_duck},
      {'c', CellContent::enemy_blue_frog},
      {'d', CellContent::enemy_blue_bat},
      {'e', CellContent::enemy_white_ghost},
      {'f', CellContent::enemy_red_flower},
      {'g', CellContent::enemy_red_bear},
      {'h', CellContent::enemy_red_coin},
      {'i', CellContent::enemy_green_fish},
      {'j', CellContent::enemy_blue_cloud},
      {'k', CellContent::enemy_blue_fog},
      {'l', CellContent::enemy_green_smiley_bouncer},
      {'m', CellContent::enemy_green_croc},
      {'n', CellContent::enemy_blue_jellyfish},
      {'o', CellContent::enemy_blue_spinning_top},
      {'p', CellContent::enemy_green_frog},
      {'q', CellContent::enemy_green_bomb},
      {'r', CellContent::enemy_green_white_centipede},
      {'s', CellContent::enemy_green_bold_roller},
      {'t', CellContent::item_bomb_increment},
      {'u', CellContent::item_bomb_reach},
      {'v', CellContent::item_speed_boost},
      {'w', CellContent::item_invincibility}}}};

struct TileData {
	std::vector<eclipse::ref<EntityInterface>> tile_content;
	bool walkable = true;
};

struct EnemyInfo {
	eclipse::Point2D position;
	EnemyType type;
};

class PlayingBoard {
public:
	explicit PlayingBoard(const std::filesystem::path& file_path, eclipse::ref<eclipse::TextureSheet> texture_sheet,
	                      const eclipse::ref<eclipse::Scene>& context);
	PlayingBoard()  = default;
	~PlayingBoard() = default;

	[[nodiscard]] size_t size() const { return board_.size(); }

	constexpr TileData& operator[](std::size_t index) { return board_[index]; }

	constexpr TileData& operator[](const eclipse::Point2D& coordinate) { return board_[get_index(coordinate)]; }

	const TileData& at(const eclipse::Point2D& coordinate) const { return board_.at(get_index(coordinate)); }

	[[nodiscard]] const std::vector<EnemyInfo>& get_enemies() const { return enemies_; }

	[[nodiscard]] constexpr const eclipse::Point2D& get_player_start_coordinates() const {
		return player_start_coordinate_;
	}

	[[nodiscard]] constexpr uint32_t get_width() const { return width_; }

	[[nodiscard]] constexpr uint32_t get_height() const { return height_; }

	[[nodiscard]] constexpr uint32_t get_level_type() const { return level_type_; }

	[[nodiscard]] bool is_coordinate_valid(const eclipse::Point2D& coordinate) const {
		return static_cast<uint32_t>(coordinate.y.in(eclipse::units::pixels) + 0.5F) <= height_ &&
		       static_cast<uint32_t>(coordinate.x.in(eclipse::units::pixels) + 0.5F) <= width_;
	}

	[[nodiscard]] bool is_index_valid(uint32_t index) const { return index < board_.size(); }

	[[nodiscard]] constexpr uint32_t get_index(const eclipse::Point2D& coordinate) const {
		return static_cast<uint32_t>(coordinate.y.in(eclipse::units::pixels) + 0.5F) * width_ +
		       static_cast<uint32_t>(coordinate.x.in(eclipse::units::pixels) + 0.5F);
	}

	[[nodiscard]] constexpr const eclipse::Point2D get_coordinates(uint32_t index) const {
		return {.x = eclipse::units::pixels(index % width_), .y = eclipse::units::pixels(index / width_)};
	}

	constexpr void clamp_coordinates(eclipse::Point2D& coordinate) { coordinate = get_coordinates(get_index(coordinate)); }

private:
	[[nodiscard]] bool is_coordinate_part_of_frame(const eclipse::Point2D& coordinate) const;
	void add_frame_to_board(au::Quantity<eclipse::units::Pixels, uint32_t> level_type);
	[[nodiscard]] void create_level_from_file(const std::filesystem::path& file_path);
	[[nodiscard]] constexpr std::optional<TileData> convert_char_to_tile_data(char file_character,
	                                                                          const eclipse::Point2D& coordinate);
	constexpr void add_enemy(const eclipse::Point2D& coordinate, char file_character);

	uint32_t level_type_;
	uint32_t width_;
	uint32_t height_;
	std::vector<TileData> board_;
	std::vector<EnemyInfo> enemies_;
	eclipse::ref<eclipse::TextureSheet> texture_sheet_;
	eclipse::ref<eclipse::Scene> context_;
	eclipse::Point2D player_start_coordinate_ {.x = eclipse::units::pixels(1), .y = eclipse::units::pixels(13)};
};

}  // namespace bomberman