#include <eclipse.h>

class ExampleLayer : public eclipse::Layer {
public:
	ExampleLayer() : Layer("Example") {}
	~ExampleLayer() = default;

	void on_update() override {
		EC_INFO_THROTTLED(0.1, "ExampleLayer::on_update");

		const unsigned int mouse_key = EC_MOUSE_BUTTON_LEFT;
		const unsigned int keyboard_key = EC_KEY_A;
		if (eclipse::InputManager::is_mouse_button_pressed(mouse_key)) {
			EC_CORE_FATAL("Mouse key is pressed!");
		}
		if (eclipse::InputManager::is_key_pressed(keyboard_key)) {
			EC_CORE_FATAL("Keyboard key is pressed!");
		}
	}

	void on_event(eclipse::Event& event) {
		//EC_TRACE("{0}", event);
	}
};

class Sandbox : public eclipse::Application {
public:
	Sandbox() {
		push_layer(new ExampleLayer()); 
		push_overlay(new eclipse::ImGuiLayer());
	}
	~Sandbox() {}
};

eclipse::Application *eclipse::create_application() {
	return new Sandbox();
}