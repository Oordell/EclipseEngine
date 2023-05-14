#pragma once

#include "core.h"
#include "window.h"
#include "eclipse/layer_stack.h"
#include "events/event.h"
#include "events/window_events.h"

#include <memory>

namespace eclipse {

	class ECLIPSE_API Application {
	public:
		Application();
		virtual ~Application();

		void run();

		void on_event(Event& e);

		void push_layer(Layer* layer);

		void push_overlay(Layer* overlay);

		inline Window& get_window() { return *window_; }

		inline static Application& get() { return *instance_; }

	private:
		bool on_window_closed(WindowClosedEvent& e);

		static Application* instance_;
		std::unique_ptr<Window> window_ = std::unique_ptr<Window>(Window::create());
		bool running_ = true;
		LayerStack layer_stack_;
	};

	// To be defined in client
	Application* create_application();

}