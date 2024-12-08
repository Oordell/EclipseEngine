#pragma once

#include <Eclipse.h>
#include <vector>
#include "common_types.hpp"

namespace bomberman {

namespace details {
static constexpr uint32_t NUM_OF_DIFFERENT_FRAME_ELEMENTS = 3;
}

enum class EnemyType {
	balloon = CellContent::enemy_balloon,
	drop    = CellContent::enemy_drop,
	striped = CellContent::enemy_striped,
	round   = CellContent::enemy_round,
	water   = CellContent::enemy_water,
	ghost   = CellContent::enemy_ghost,
	bear    = CellContent::enemy_bear,
	coin    = CellContent::enemy_coin
};

static constexpr eclipse::Map<char, CellContent, 16> char_to_cell_content = {
    {{{'_', CellContent::solid_wall},
      {'b', CellContent::grass},
      {'c', CellContent::soft_wall},
      {'d', CellContent::start_pose},
      {'e', CellContent::enemy_balloon},
      {'f', CellContent::enemy_drop},
      {'g', CellContent::enemy_striped},
      {'h', CellContent::enemy_round},
      {'i', CellContent::enemy_water},
      {'j', CellContent::enemy_ghost},
      {'k', CellContent::enemy_bear},
      {'l', CellContent::enemy_coin},
      {'m', CellContent::item_bomb_increment},
      {'n', CellContent::item_bomb_reach},
      {'o', CellContent::item_speed_boost},
      {'p', CellContent::item_invincibility}}}};

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
	explicit PlayingBoard(const std::filesystem::path& file_path, eclipse::ref<eclipse::TextureSheet> texture_sheet_frames,
	                      eclipse::ref<eclipse::TextureSheet> texture_sheet_items,
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
	void add_frame_to_board();
	[[nodiscard]] void create_level_from_file(const std::filesystem::path& file_path);
	[[nodiscard]] constexpr std::optional<TileData> convert_char_to_tile_data(char file_character,
	                                                                          const eclipse::Point2D& coordinate);
	constexpr void add_enemy(const eclipse::Point2D& coordinate, char file_character);

	uint32_t level_type_;
	uint32_t width_;
	uint32_t height_;
	std::vector<TileData> board_;
	std::vector<EnemyInfo> enemies_;
	eclipse::ref<eclipse::TextureSheet> texture_sheet_frames_;
	eclipse::ref<eclipse::TextureSheet> texture_sheet_items_;
	eclipse::ref<eclipse::Scene> context_;
	eclipse::Point2D player_start_coordinate_ {.x = eclipse::units::pixels(1), .y = eclipse::units::pixels(13)};
};

}  // namespace bomberman