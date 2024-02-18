#include "game_layer.h"

GameLayer::GameLayer() : Layer("GameLayer") {
	auto& window = eclipse::Application::get().get_window();
	create_camera({.width = window.get_width(), .height = window.get_height()});

	Random::init();
}

void GameLayer::on_attach() {
	level_.init();
	ImGuiIO io = ImGui::GetIO();
	font_      = io.Fonts->AddFontFromFileTTF("assets/OpenSans-Regular.ttf", 20.0F);
}

void GameLayer::on_detach() {}

void GameLayer::on_update(eclipse::Timestep timestep) {
	time_ += timestep;

	if (static_cast<int>(time_ * 10.0f) % 8 > 4) {
		blink_ = !blink_;
	}

	if (level_.is_game_over()) {
		game_state_ = GameState::game_over;
	}

	const auto& player_pos = level_.get_player().get_position();
	camera_->set_position({player_pos.x, player_pos.y, 0.0F});

	switch (game_state_) {
		case GameState::play: {
			level_.on_update(timestep);
			break;
		}
	}

	// Render
	eclipse::RenderCommand::set_clear_color({0.0F, 0.0F, 0.0F, 1.0F});
	eclipse::RenderCommand::clear();

	eclipse::Renderer2D::begin_scene(*camera_);
	level_.on_render();
	eclipse::Renderer2D::end_scene();
}

void GameLayer::on_event(eclipse::Event& event) {
	eclipse::EventDispatcher dispatcher(event);
	dispatcher.dispatch<eclipse::WindowResizeEvent>(EC_BIND_EVENT_FN(GameLayer::on_window_resize));
	dispatcher.dispatch<eclipse::MouseButtonPressedEvent>(EC_BIND_EVENT_FN(GameLayer::on_mouse_button_pressed));
	dispatcher.dispatch<eclipse::MouseScrolledEvent>(EC_BIND_EVENT_FN(GameLayer::on_mouse_scrolled));
}

void GameLayer::on_imgui_render() {
	ImGui::Begin("Settings");
	level_.on_imgui_render();
	ImGui::End();

	using enum GameState;
	auto window_pose = ImGui::GetWindowPos();
	switch (game_state_) {
		case play: {
			add_imgui_text_player_score(window_pose);
			break;
		}
		case main_menu: {
			add_imgui_text_click_to_play(window_pose);
			break;
		}
		case game_over: {
			add_imgui_text_click_to_play(window_pose);

			window_pose.x += 200.0f;
			window_pose.y += 150.0f;
			add_imgui_text_player_score(window_pose);
			break;
		}
	}
}

void GameLayer::add_imgui_text_player_score(ImVec2& window_pose) {
	uint32_t player_score = level_.get_player().get_score();
	std::string score_str = std::string("Score: ") + std::to_string(player_score);
	ImGui::GetForegroundDrawList()->AddText(font_, 48.0f, window_pose, 0xffffffff, score_str.c_str());
}

void GameLayer::add_imgui_text_click_to_play(ImVec2& window_pose) {
	auto width = eclipse::Application::get().get_window().get_width();
	window_pose.x -= width * 0.5F;
	if (blink_) {
		ImGui::GetForegroundDrawList()->AddText(font_, 120.0f, window_pose, 0xffffffff, "Click to Play!");
	}
}

bool GameLayer::on_mouse_button_pressed(eclipse::MouseButtonPressedEvent& e) {
	if (game_state_ == GameState::game_over) {
		level_.reset();
	}

	game_state_ = GameState::play;
	return false;
}

bool GameLayer::on_window_resize(eclipse::WindowResizeEvent& e) {
	create_camera(e.get_window_size());
	return false;
}

bool GameLayer::on_mouse_scrolled(eclipse::MouseScrolledEvent& e) {
	zoom_level_ -= e.get_y_offset() * 0.1F;
	zoom_level_ = std::max(zoom_level_, 0.25F);
	camera_->set_projection({.left   = -aspect_ratio_ * zoom_level_,
	                         .right  = aspect_ratio_ * zoom_level_,
	                         .bottom = -zoom_level_,
	                         .top    = zoom_level_});
	return false;
}

void GameLayer::create_camera(const eclipse::WindowSize& size) {
	aspect_ratio_      = size.get_aspect_ratio();
	float camera_width = 8.0F;
	float bottom       = -camera_width;
	float top          = camera_width;
	float left         = bottom * aspect_ratio_;
	float right        = top * aspect_ratio_;
	camera_            = eclipse::make_scope<eclipse::OrthographicCamera>(
     eclipse::OrthograficLimits {.left = left, .right = right, .bottom = bottom, .top = top});
}
