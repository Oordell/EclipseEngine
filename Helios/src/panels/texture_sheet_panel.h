#pragma once

#include "Eclipse.h"
#include "entity_destroyed_interface.h"

namespace eclipse {

class TextureSheetPanel {
public:
	TextureSheetPanel() = default;
	TextureSheetPanel(const ref<Scene>& context);
	void set_context(const ref<Scene>& context);
	void on_imgui_render();

	void add_entity_destroyed_subscriber(EntityDestroyedInterface* subscriber) {
		entity_destroyed_subscribers_.push_back(subscriber);
	}

	void remove_entity_destroyed_subscriber(EntityDestroyedInterface* subscriber) {
		auto it = std::find(entity_destroyed_subscribers_.begin(), entity_destroyed_subscribers_.end(), subscriber);
		if (it != entity_destroyed_subscribers_.end()) {
			entity_destroyed_subscribers_.erase(it);
		}
	}

private:
	void draw_add_texture_sheet_button();
	void draw_texture_sheet_list();
	void destroy_entities_that_uses_sheet(Entity texture_sheet_entity);

	void notify_subscribers(Entity entity) {
		for (auto& sub : entity_destroyed_subscribers_) {
			sub->on_entity_destroyed(entity);
		}
	}

	ref<Scene> context_;
	std::vector<EntityDestroyedInterface*> entity_destroyed_subscribers_ {};
	static constexpr uint32_t MAX_TEXTURE_SHEETS = 8;
};

}  // namespace eclipse