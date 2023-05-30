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
	void on_imgui_render() override;

	void begin();
	void end();
};

}  // namespace eclipse