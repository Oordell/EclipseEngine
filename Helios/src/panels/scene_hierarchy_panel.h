#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <concepts>

#include "Eclipse.h"
#include "panel_utils.h"
#include "entity_destroyed_interface.h"

namespace eclipse {

class SceneHierarchyPanel : public EntityDestroyedInterface {
public:
	SceneHierarchyPanel()  = default;
	~SceneHierarchyPanel() = default;
	SceneHierarchyPanel(const ref<Scene>& context);
	void on_entity_destroyed(Entity e) override;
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
	void draw_circle_collider_2d_component(Entity entity);
	void draw_sub_texture_component(Entity entity);

	template <component::IsComponent Component>
	void add_pop_up_option(Entity entity, const std::string& menu_item);

	ref<Scene> context_;
	Entity selection_context_;
};

}  // namespace eclipse