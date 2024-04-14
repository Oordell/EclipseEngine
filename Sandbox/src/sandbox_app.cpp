#include <eclipse.h>
#include <eclipse/core/entry_point.h>

#include "sandbox_2d.h"
#include "example_game_1/game_layer.h"

class Sandbox : public eclipse::Application {
public:
	Sandbox(const eclipse::ApplicationCommandLineArgs& args)
	    : Application({.title       = "Sandbox App",
	                   .window_size = {.width = eclipse::units::pixels(1600), .height = eclipse::units::pixels(900)}},
	                  args) {
		//	push_layer(new ExampleLayer());
		push_layer(new Sandbox2D());
		//	push_layer(new GameLayer());
	}

	~Sandbox() {}
};

eclipse::Application* eclipse::create_application(const ApplicationCommandLineArgs& args) { return new Sandbox(args); }