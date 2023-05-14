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
		void on_update() override;
		void on_event(Event& event) override;

	private:
		bool on_mouse_button_pressed_event(MouseButtonPressedEvent& e);
		bool on_mouse_button_released_event(MouseButtonReleasedEvent& e);
		bool on_mouse_moved_event(MouseMovedEvent& e);
		bool on_mouse_scrolled_event(MouseScrolledEvent& e);
		bool on_key_pressed_event(KeyPressedEvent& e);
		bool on_key_released_event(KeyReleasedEvent& e);
		bool on_key_typed_event(KeyTypedEvent& e);
		bool on_window_resize_event(WindowResizeEvent& e);

		float time_ = 0.0F;
	};

}