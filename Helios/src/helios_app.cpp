#include <eclipse.h>
#include <eclipse/core/entry_point.h>

#include "editor_layer.h"

namespace eclipse {

class Helios : public Application {
public:
	Helios(const ApplicationCommandLineArgs& args)
	    : Application({.title       = "Helios (Eclipse Editor)",
	                   .window_size = {.width = units::pixels(1600), .height = units::pixels(900)}},
	                  args) {
		push_layer(new EditorLayer());
	}

	~Helios() {}
};

Application* create_application(const ApplicationCommandLineArgs& args) { return new Helios(args); }

}  // namespace eclipse