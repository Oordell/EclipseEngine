#include <eclipse.h>
#include <eclipse/core/entry_point.h>

#include "editor_layer.h"

namespace eclipse {

class Helios : public Application {
public:
	Helios() : Application("Helios (Eclipse Editor)") { push_layer(new EditorLayer()); }

	~Helios() {}
};

Application* create_application() { return new Helios(); }

}  // namespace eclipse