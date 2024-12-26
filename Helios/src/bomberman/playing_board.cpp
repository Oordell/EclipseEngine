#include "ecpch.h"
#include "playing_board.hpp"
#include "solid_wall.hpp"
#include "soft_wall.hpp"
#include "item.hpp"
#include "enemy.hpp"
#include "frame.hpp"

namespace bomberman {
PlayingBoard::PlayingBoard(const std::filesystem::path& file_path, eclipse::ref<eclipse::TextureSheet> texture_sheet,
                           const eclipse::ref<eclipse::Scene>& context)
    : texture_sheet_(texture_sheet), context_(context) {
	create_level_from_file(file_path);
}

bool PlayingBoard::is_coordinate_part_of_frame(const eclipse::Point2D& coordinate) const {
	return coordinate.x.in(eclipse::units::pixels) == 0 || coordinate.x.in(eclipse::units::pixels) == width_ - 1 ||
	       coordinate.y.in(eclipse::units::pixels) == 0 || coordinate.y.in(eclipse::units::pixels) == height_ - 1;
}

void PlayingBoard::add_frame_to_board(au::Quantity<eclipse::units::Pixels, uint32_t> level_type) {
	using eclipse::units::pixels;
	eclipse::Point2D coordinate {.x = pixels(0.F), .y = pixels(0.F)};

	for (int i = 1; i < width_ - 1; i++) {
		coordinate.x      = pixels(static_cast<float>(i));
		coordinate.y      = pixels(static_cast<float>(height_ - 1));
		auto frame_toggle = i % details::NUM_OF_DIFFERENT_FRAME_ELEMENTS;
		if (frame_toggle == 1) {
			board_[get_index(coordinate)] =
			    TileData {.tile_content = {eclipse::make_ref<FrameTop1>(texture_sheet_, coordinate, context_, level_type)},
			              .walkable     = false};
		} else if (frame_toggle == 2) {
			board_[get_index(coordinate)] =
			    TileData {.tile_content = {eclipse::make_ref<FrameTop2>(texture_sheet_, coordinate, context_, level_type)},
			              .walkable     = false};
		} else {
			board_[get_index(coordinate)] =
			    TileData {.tile_content = {eclipse::make_ref<FrameTop3>(texture_sheet_, coordinate, context_, level_type)},
			              .walkable     = false};
		}

		coordinate.y = pixels(0.F);
		if (frame_toggle == 1) {
			board_[get_index(coordinate)] =
			    TileData {.tile_content = {eclipse::make_ref<FrameBottom1>(texture_sheet_, coordinate, context_, level_type)},
			              .walkable     = false};
		} else if (frame_toggle == 2) {
			board_[get_index(coordinate)] =
			    TileData {.tile_content = {eclipse::make_ref<FrameBottom2>(texture_sheet_, coordinate, context_, level_type)},
			              .walkable     = false};
		} else {
			board_[get_index(coordinate)] =
			    TileData {.tile_content = {eclipse::make_ref<FrameBottom3>(texture_sheet_, coordinate, context_, level_type)},
			              .walkable     = false};
		}
	}

	for (int i = 0; i < height_ - 1; i++) {
		coordinate.x      = pixels(static_cast<float>(width_ - 1));
		coordinate.y      = pixels(static_cast<float>(i));
		auto frame_toggle = i % details::NUM_OF_DIFFERENT_FRAME_ELEMENTS;

		if (frame_toggle == 1) {
			board_[get_index(coordinate)] =
			    TileData {.tile_content = {eclipse::make_ref<FrameRight1>(texture_sheet_, coordinate, context_, level_type)},
			              .walkable     = false};
		} else if (frame_toggle == 0) {
			board_[get_index(coordinate)] =
			    TileData {.tile_content = {eclipse::make_ref<FrameRight2>(texture_sheet_, coordinate, context_, level_type)},
			              .walkable     = false};
		} else {
			board_[get_index(coordinate)] =
			    TileData {.tile_content = {eclipse::make_ref<FrameRight3>(texture_sheet_, coordinate, context_, level_type)},
			              .walkable     = false};
		}

		coordinate.x = pixels(0.F);
		if (frame_toggle == 1) {
			board_[get_index(coordinate)] =
			    TileData {.tile_content = {eclipse::make_ref<FrameLeft1>(texture_sheet_, coordinate, context_, level_type)},
			              .walkable     = false};
		} else if (frame_toggle == 0) {
			board_[get_index(coordinate)] =
			    TileData {.tile_content = {eclipse::make_ref<FrameLeft2>(texture_sheet_, coordinate, context_, level_type)},
			              .walkable     = false};
		} else {
			board_[get_index(coordinate)] =
			    TileData {.tile_content = {eclipse::make_ref<FrameLeft3>(texture_sheet_, coordinate, context_, level_type)},
			              .walkable     = false};
		}
	}

	// Corners of the frame
	coordinate                    = {.x = pixels(0.F), .y = pixels(0.F)};
	board_[get_index(coordinate)] = TileData {
	    .tile_content = {eclipse::make_ref<FrameCornerBottomLeft>(texture_sheet_, coordinate, context_, level_type)},
	    .walkable     = false};
	coordinate                    = {.x = pixels(width_ - 1), .y = pixels(0.F)};
	board_[get_index(coordinate)] = TileData {
	    .tile_content = {eclipse::make_ref<FrameCornerBottomRight>(texture_sheet_, coordinate, context_, level_type)},
	    .walkable     = false};
	coordinate                    = {.x = pixels(0.F), .y = pixels(height_ - 1)};
	board_[get_index(coordinate)] = TileData {
	    .tile_content = {eclipse::make_ref<FrameCornerTopLeft>(texture_sheet_, coordinate, context_, level_type)},
	    .walkable     = false};
	coordinate                    = {.x = pixels(width_ - 1), .y = pixels(height_ - 1)};
	board_[get_index(coordinate)] = TileData {
	    .tile_content = {eclipse::make_ref<FrameCornerTopRight>(texture_sheet_, coordinate, context_, level_type)},
	    .walkable     = false};
}

void PlayingBoard::create_level_from_file(const std::filesystem::path& file_path) {
	std::ifstream file(file_path.string());
	if (!file.is_open()) {
		EC_ERROR("Error: Could not open file {}", file_path.string());
		return;
	}

	// Read level
	if (!(file >> level_type_)) {
		EC_ERROR("Error: Failed to read level.");
		return;
	}

	// Read width
	if (!(file >> width_)) {
		EC_ERROR("Error: Failed to read width.");
		return;
	}

	// Read height
	if (!(file >> height_)) {
		EC_ERROR("Error: Failed to read height.");
		return;
	}

	board_.resize(width_ * height_);
	add_frame_to_board(pixels(level_type_));

	// Read the grid
	eclipse::Point2D coordinate;
	for (int32_t y = height_ - 1; y >= 0; --y) {
		coordinate.y = eclipse::units::pixels(y);
		for (uint32_t x = 0; x < width_; ++x) {
			coordinate.x = eclipse::units::pixels(x);
			char tile;
			char separator;

			if (!(file >> tile)) {
				EC_ERROR("Error: Invalid level data at position ({}, {})", y, x);
				return;
			}

			if (!is_coordinate_part_of_frame(coordinate)) {
				auto tile_data = convert_char_to_tile_data(tile, coordinate);
				if (char_to_cell_content.at(tile) == CellContent::start_pose) {
					player_start_coordinate_ = coordinate;
				} else if (tile_data.has_value()) {
					board_[get_index(coordinate)] = tile_data.value();
				} else if (char_to_cell_content.at(tile) != CellContent::grass) {
					add_enemy(coordinate, tile);
				}
			}
			// Handle comma separator if it's not the last element in the row
			if (x < width_ - 1 && !(file >> separator) && separator != ',') {
				EC_ERROR("Error: Expected ',' between elements.");
				return;
			}
		}
	}
}

constexpr std::optional<TileData> PlayingBoard::convert_char_to_tile_data(char file_character,
                                                                          const eclipse::Point2D& coordinate) {
	using enum CellContent;
	auto cell_type = char_to_cell_content.at(file_character);
	TileData tile;

	switch (cell_type) {
		case wall_solid: {
			tile.tile_content.push_back(eclipse::make_ref<SolidWall>(texture_sheet_, coordinate, context_, pixels(level_type_)));
			tile.walkable = false;
			return tile;
		}
		case wall_soft: {
			tile.tile_content.push_back(eclipse::make_ref<SoftWall>(texture_sheet_, coordinate, context_, pixels(level_type_)));
			tile.walkable = false;
			return tile;
		}
		case item_bomb_increment: {
			tile.tile_content.push_back(eclipse::make_ref<ItemBombIncrement>(texture_sheet_, coordinate, context_));
			tile.tile_content.push_back(eclipse::make_ref<SoftWall>(texture_sheet_, coordinate, context_, pixels(level_type_)));
			tile.walkable = false;
			return tile;
		}
		case item_bomb_reach: {
			tile.tile_content.push_back(eclipse::make_ref<ItemBombReach>(texture_sheet_, coordinate, context_));
			tile.tile_content.push_back(eclipse::make_ref<SoftWall>(texture_sheet_, coordinate, context_, pixels(level_type_)));
			tile.walkable = false;
			return tile;
		}
		case item_invincibility: {
			tile.tile_content.push_back(eclipse::make_ref<ItemInvincibility>(texture_sheet_, coordinate, context_));
			tile.tile_content.push_back(eclipse::make_ref<SoftWall>(texture_sheet_, coordinate, context_, pixels(level_type_)));
			tile.walkable = false;
			return tile;
		}
		case item_speed_boost: {
			tile.tile_content.push_back(eclipse::make_ref<ItemSpeedBoost>(texture_sheet_, coordinate, context_));
			tile.tile_content.push_back(eclipse::make_ref<SoftWall>(texture_sheet_, coordinate, context_, pixels(level_type_)));
			tile.walkable = false;
			return tile;
		}
	}

	return std::nullopt;
}

constexpr void PlayingBoard::add_enemy(const eclipse::Point2D& coordinate, char file_character) {
	using enum CellContent;
	auto cell_type = char_to_cell_content.at(file_character);
	if (cell_type == enemy_red_balloon || cell_type == enemy_green_duck || cell_type == enemy_blue_frog ||
	    cell_type == enemy_blue_bat || cell_type == enemy_white_ghost || cell_type == enemy_red_flower ||
	    cell_type == enemy_red_bear || cell_type == enemy_red_coin || cell_type == enemy_green_fish ||
	    cell_type == enemy_blue_cloud || cell_type == enemy_blue_fog || cell_type == enemy_green_smiley_bouncer ||
	    cell_type == enemy_green_croc || cell_type == enemy_blue_jellyfish || cell_type == enemy_blue_spinning_top ||
	    cell_type == enemy_green_frog || cell_type == enemy_green_bomb || cell_type == enemy_green_white_centipede ||
	    cell_type == enemy_green_bold_roller) {
		enemies_.push_back({.position = coordinate, .type = static_cast<EnemyType>(cell_type)});
		return;
	}

	EC_ERROR("Unsupported enemy type: {}, from char: {}", static_cast<int>(cell_type), file_character);
}

}  // namespace bomberman