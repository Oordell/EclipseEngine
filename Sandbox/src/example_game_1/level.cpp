#include "level.h"

#include <glm/gtc/matrix_transform.hpp>

void Level::init() {
	triangle_texture_ = eclipse::Texture2D::create("assets/textures/triangle.png");
	player_.load_asserts();
	pillars_.resize(5);
	for (int i = 0; i < NUM_OF_PILARS; i++) {
		create_pillar(i, i * PILAR_OFFSET_MULTIPLIER);
	}
}

void Level::on_update(eclipse::Timestep timestep) {
	player_.on_update(timestep);

	if (collision_check()) {
		game_over();
		return;
	}

	pillar_hsv_.hue += 0.05F * timestep;
	if (pillar_hsv_.hue > 1.0F) {
		pillar_hsv_.hue = 0.0F;
	}

	if (player_.get_position().x > pillar_target_) {
		create_pillar(pillar_index_, pillar_target_ + 20.0F);
		pillar_index_ = ++pillar_index_ % pillars_.size();
		pillar_target_ += 10.0F;
	}
}

void Level::on_render() {
	const auto& player_pose = player_.get_position();
	auto color              = hsv_to_rgb(pillar_hsv_).get();

	// Background
	eclipse::Renderer2D::draw_quad({.spatial_info = {.position = {player_pose.x, 0.0F, -0.8F}, .size = {50.0F, 50.0F}},
	                                .common       = {.color = {0.3F, 0.3F, 0.3F, 1.0F}}});

	// Floor & ceiling
	eclipse::Renderer2D::draw_quad(
	    {.spatial_info = {.position = {player_pose.x, 34.0F}, .size = {50.0F, 50.0F}}, .common = {.color = color}});
	eclipse::Renderer2D::draw_quad(
	    {.spatial_info = {.position = {player_pose.x, -34.0F}, .size = {50.0F, 50.0F}}, .common = {.color = color}});

	for (auto& pillar : pillars_) {
		eclipse::Renderer2D::draw_quad(
		    eclipse::QuadMetaDataPosition3DTexture {.spatial_info = {.position     = pillar.top_position,
		                                                             .rotation_rad = eclipse::utils::deg_to_rad(180.0F),
		                                                             .size         = pillar.top_scale},
		                                            .common       = {.color = color},
		                                            .texture      = triangle_texture_});
		eclipse::Renderer2D::draw_quad(eclipse::QuadMetaDataPosition3DTexture {
		    .spatial_info = {.position = pillar.bottom_position, .rotation_rad = 0.0F, .size = pillar.bottom_scale},
		    .common       = {.color = color},
		    .texture      = triangle_texture_});
	}

	player_.on_render();
}

void Level::on_imgui_render() { player_.on_imgui_render(); }

void Level::reset() {
	game_over_ = false;
	player_.reset();
	pillar_target_ = 30.0f;
	pillar_index_  = 0;
	for (int i = 0; i < NUM_OF_PILARS; i++) {
		create_pillar(i, i * PILAR_OFFSET_MULTIPLIER);
	}
}

void Level::create_pillar(int index, float offset) {
	Pillar& pillar           = pillars_[index];
	pillar.top_position.x    = offset;
	pillar.bottom_position.x = offset;
	pillar.top_position.z    = index * 0.1f - 0.5f;
	pillar.bottom_position.z = index * 0.1f - 0.5f + 0.05f;

	float center = Random::get_float() * 35.0f - 17.5f;
	float gap    = 2.0f + Random::get_float() * 5.0f;

	pillar.top_position.y    = 10.0f - ((10.0f - center) * 0.2f) + gap * 0.5f;
	pillar.bottom_position.y = -10.0f - ((-10.0f - center) * 0.2f) - gap * 0.5f;
}

bool Level::collision_check() {
	static const float Y_THRESHOLD = 8.5F;

	if (glm::abs(player_.get_position().y) > Y_THRESHOLD) {
		return true;
	}

	/* clang-format off */
	static const uint32_t NUM_OF_PLAYER_VERTICES      = 4;
	glm::vec4 player_vertices[NUM_OF_PLAYER_VERTICES] = {
	    {-0.5F, -0.5F, 0.0F, 1.0F}, 
		{0.5F, -0.5F, 0.0F, 1.0F}, 
		{0.5F, 0.5F, 0.0F, 1.0F}, 
		{-0.5F, 0.5F, 0.0F, 1.0F}
	};
	/* clang-format on */

	static const glm::mat4 IDENTITY_MATRIX = glm::mat4(1.0F);
	const auto& pos                        = player_.get_position();
	glm::vec4 player_transformed_verts[4];
	for (int i = 0; i < NUM_OF_PLAYER_VERTICES; i++) {
		player_transformed_verts[i] = glm::translate(IDENTITY_MATRIX, {pos.x, pos.y, 0.0F}) *
		                              glm::rotate(IDENTITY_MATRIX, glm::radians(player_.get_rotation()), {0.0F, 0.0F, 1.0F}) *
		                              glm::scale(IDENTITY_MATRIX, {1.0F, 1.3F, 1.0F}) * player_vertices[i];
	}

	// To match triangle.png (each corner is 10% from the texture edge)
	static const uint32_t NUM_OF_PILLAR_VERTICES      = 3;
	glm::vec4 pillar_vertices[NUM_OF_PILLAR_VERTICES] = {
	    {-0.5F + 0.1F, -0.5F + 0.1F, 0.0F, 1.0F},
	    {0.5F - 0.1F, -0.5F + 0.1F, 0.0F, 1.0F},
	    {0.0F + 0.0F, 0.5F - 0.1F, 0.0F, 1.0F},
	};

	static const uint32_t NUM_OF_PILLAR_CORNERS = 3;
	for (auto& p : pillars_) {
		glm::vec2 triangle[NUM_OF_PILLAR_CORNERS];

		// Top pillars
		for (int i = 0; i < NUM_OF_PILLAR_CORNERS; i++) {
			triangle[i] = glm::translate(IDENTITY_MATRIX, {p.top_position.x, p.top_position.y, 0.0F}) *
			              glm::rotate(IDENTITY_MATRIX, glm::radians(180.0F), {0.0F, 0.0F, 1.0F}) *
			              glm::scale(IDENTITY_MATRIX, {p.top_scale.x, p.top_scale.y, 1.0F}) * pillar_vertices[i];
		}

		for (auto& vert : player_transformed_verts) {
			if (point_in_triangle({vert.x, vert.y}, triangle[0], triangle[1], triangle[2])) {
				return true;
			}
		}

		// Bottom pillars
		for (int i = 0; i < NUM_OF_PILLAR_CORNERS; i++) {
			triangle[i] = glm::translate(IDENTITY_MATRIX, {p.bottom_position.x, p.bottom_position.y, 0.0F}) *
			              glm::scale(IDENTITY_MATRIX, {p.bottom_scale.x, p.bottom_scale.y, 1.0F}) * pillar_vertices[i];
		}

		for (auto& vert : player_transformed_verts) {
			if (point_in_triangle({vert.x, vert.y}, triangle[0], triangle[1], triangle[2])) {
				return true;
			}
		}
	}

	return false;
}

void Level::game_over() { game_over_ = true; }

bool Level::point_in_triangle(const glm::vec2& point, glm::vec2& corner_0, const glm::vec2& corner_1,
                              const glm::vec2& corner_2) {
	float s = corner_0.y * corner_2.x - corner_0.x * corner_2.y + (corner_2.y - corner_0.y) * point.x +
	          (corner_0.x - corner_2.x) * point.y;
	float t = corner_0.x * corner_1.y - corner_0.y * corner_1.x + (corner_0.y - corner_1.y) * point.x +
	          (corner_1.x - corner_0.x) * point.y;

	if ((s < 0) != (t < 0)) {
		return false;
	}

	float A = -corner_1.y * corner_2.x + corner_0.y * (corner_2.x - corner_1.x) + corner_0.x * (corner_1.y - corner_2.y) +
	          corner_1.x * corner_2.y;

	return A < 0 ? (s <= 0 && s + t >= A) : (s >= 0 && s + t <= A);
}
