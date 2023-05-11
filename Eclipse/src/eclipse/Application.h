#pragma once

#include "Core.h"
#include "Window.h"
#include "eclipse/LayerStack.h"
#include "Events/Event.h"
#include "Events/WindowEvents.h"

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

	private:
		bool on_window_closed(WindowClosedEvent& e);

		std::unique_ptr<Window> window_ = std::unique_ptr<Window>(Window::create());
		bool running_ = true;
		LayerStack layer_stack_;
	};

	// To be defined in client
	Application* create_application();

}