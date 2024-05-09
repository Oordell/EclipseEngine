#pragma once

#include "Eclipse.h"

namespace eclipse {

class SceneHierarchyPanel {
public:
	SceneHierarchyPanel() = default;
	SceneHierarchyPanel(const ref<Scene>& context);
	void set_context(const ref<Scene>& context);
	void on_imgui_render();
	void set_selected_entity(Entity entity);

	[[nodiscard]] Entity get_selected_entity() const { return selection_context_; }

private:
	void draw_entity_node(Entity entity);
	void draw_components(Entity entity);
	void draw_tag_component(Entity entity);
	void draw_transform_component(Entity entity);
	void draw_camera_component(Entity entity);
	void draw_color_component(Entity entity);
	void draw_sprite_renderer_component(Entity entity);
	void draw_circle_renderer_component(Entity entity);
	void draw_rigid_body_2d_component(Entity entity);
	void draw_box_collider_2d_component(Entity entity);

	template <class Component>
	void add_pop_up_option(Entity entity, const std::string& menu_item);

	struct Vec3Controls {
		glm::vec3& values;
		const std::string& label = "";
		float reset_value        = 0.0F;
		float column_width       = 100.0F;
	};

	void draw_vec3_control(const Vec3Controls& controls);

	ref<Scene> context_;
	Entity selection_context_;
};

}  // namespace eclipse