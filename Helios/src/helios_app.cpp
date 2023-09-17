#include <eclipse.h>
#include <eclipse/core/entry_point.h>

#include "editor_layer.h"

namespace eclipse {

class Helios : public Application {
public:
	Helios() : Application({.title = "Helios (Eclipse Editor)", .window_size = {.width = 1600, .height = 900}}) {
		push_layer(new EditorLayer());
	}

	~Helios() {}
};

Application* create_application() { return new Helios(); }

}  // namespace eclipse