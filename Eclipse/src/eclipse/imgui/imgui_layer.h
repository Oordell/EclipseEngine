#pragma once

#include "eclipse/layer/layer.h"
#include "eclipse/events/mouse_events.h"
#include "eclipse/events/key_events.h"
#include "eclipse/events/window_events.h"

namespace eclipse {

class ECLIPSE_API ImGuiLayer : public Layer {
public:
	ImGuiLayer();
	~ImGuiLayer();

	void on_attach() override;
	void on_detach() override;
	void on_event(Event& e) override;
	void begin();
	void end();

	void set_dark_theme_colors();

	void set_block_events(bool block) { block_events_ = block; }

private:
	bool block_events_ {false};
};

}  // namespace eclipse