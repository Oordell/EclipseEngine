#pragma once

#include "core.h"
#include "window.h"
#include "eclipse/layer/layer_stack.h"
#include "events/event.h"
#include "events/window_events.h"
#include "eclipse/imgui/imgui_layer.h"

#include <memory>

#include "eclipse/renderer/shader.h"

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
		std::shared_ptr<ImGuiLayer> imgui_layer_ = std::make_shared<ImGuiLayer>();
		bool running_ = true;
		LayerStack layer_stack_;

		unsigned int vertex_array_ = 0;
		unsigned int vertex_buffer_ = 0;
		unsigned int index_buffer_ = 0;
		std::unique_ptr<Shader> shader_;
	};

	// To be defined in client
	Application* create_application();

}