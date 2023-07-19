#pragma once

#include "eclipse.h"
#include "pillar.h"
#include "color.h"
#include "random.h"
#include "player.h"

class Level {
public:
	void init();
	void on_update(eclipse::Timestep timestep);
	void on_render();
	void on_imgui_render();
	void reset();

	Player& get_player() { return player_; }

	bool is_game_over() const { return game_over_; }

private:
	void create_pillar(int index, float offset);
	bool collision_check();
	void game_over();
	static bool point_in_triangle(const glm::vec2& point, glm::vec2& corner_0, const glm::vec2& corner_1,
	                              const glm::vec2& corner_2);

	Player player_;
	bool game_over_      = false;
	float pillar_target_ = 30.0f;
	int pillar_index_    = 0;
	ColorHSV pillar_hsv_ = {.hue = 0.0F, .sat = 0.8F, .val = 0.8F};
	std::vector<Pillar> pillars_;
	eclipse::ref<eclipse::Texture2D> triangle_texture_;

	static constexpr uint16_t NUM_OF_PILARS        = 5;
	static constexpr float PILAR_OFFSET_MULTIPLIER = 10.0F;
};