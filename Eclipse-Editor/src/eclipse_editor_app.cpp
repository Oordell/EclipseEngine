#include <eclipse.h>
#include <eclipse/core/entry_point.h>

#include "editor_layer.h"

namespace eclipse {

class EclipseEditor : public Application {
public:
	EclipseEditor() : Application("Eclipse Editor") { push_layer(new EditorLayer()); }

	~EclipseEditor() {}
};

Application* create_application() { return new EclipseEditor(); }

}  // namespace eclipse