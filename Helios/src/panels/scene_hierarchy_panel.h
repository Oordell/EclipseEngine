#pragma once

#include "Eclipse.h"

namespace eclipse {

class SceneHierarchyPanel {
public:
	SceneHierarchyPanel() = default;
	SceneHierarchyPanel(const ref<Scene>& context);
	void set_context(const ref<Scene>& context);
	void on_imgui_render();

private:
	void draw_entity_node(Entity entity);
	void draw_components(Entity entity);

	ref<Scene> context_;
	Entity selection_context_;
};

}  // namespace eclipse