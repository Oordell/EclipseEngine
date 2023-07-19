#pragma once

#include "Eclipse.h"
#include "level.h"

#include <imgui/imgui.h>

class GameLayer : public eclipse::Layer {
public:
	GameLayer();
	~GameLayer() override = default;
	void on_attach() override;
	void on_detach() override;
	void on_update(eclipse::Timestep timestep) override;
	void on_event(eclipse::Event& event) override;
	void on_imgui_render() override;
	void add_imgui_text_player_score(ImVec2& window_pose);
	void add_imgui_text_click_to_play(ImVec2& window_pose);
	bool on_mouse_button_pressed(eclipse::MouseButtonPressedEvent& e);
	bool on_window_resize(eclipse::WindowResizeEvent& e);
	bool on_mouse_scrolled(eclipse::MouseScrolledEvent& e);

private:
	void create_camera(const eclipse::WindowSize& size);

	eclipse::scope<eclipse::OrthographicCamera> camera_;
	float time_ {0.0F};
	bool blink_ {false};
	Level level_;
	ImFont* font_;
	float zoom_level_ {8.0F};
	float aspect_ratio_;

	enum class GameState { play, main_menu, game_over };
	GameState game_state_ = GameState::main_menu;
};