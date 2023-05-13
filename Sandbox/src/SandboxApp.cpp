#include <Eclipse.h>

class ExampleLayer : public eclipse::Layer {
public:
	ExampleLayer() : Layer("Example") {}
	~ExampleLayer() = default;

	void on_update() override {
		EC_INFO("ExampleLayer::on_update");
	}

	void on_event(eclipse::Event& event) {
		EC_TRACE("{0}", event);
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