#include <eclipse.h>
#include <eclipse/core/entry_point.h>

#include "sandbox_2d.h"

class Sandbox : public eclipse::Application {
public:
	Sandbox() {
		//	push_layer(new ExampleLayer());
		push_layer(new Sandbox2D());
	}

	~Sandbox() {}
};

eclipse::Application* eclipse::create_application() { return new Sandbox(); }