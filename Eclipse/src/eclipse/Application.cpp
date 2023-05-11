#include "ecpch.h"
#include "Application.h"

#include <glad/glad.h>

namespace eclipse {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application() {
		window_->set_event_callback(BIND_EVENT_FN(on_event));
	};

	Application::~Application() {};

	void Application::on_event(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowClosedEvent>(BIND_EVENT_FN(on_window_closed));

		EC_CORE_INFO("{0}", e);

		for (auto it = layer_stack_.end(); it != layer_stack_.begin(); ) {
			(*--it)->on_event(e);
			if (e.handled) {
				break;
			}
		}
	}

	void Application::push_layer(Layer* layer) {
		layer_stack_.push_layer(layer);
	}

	void Application::push_overlay(Layer* overlay) {
		layer_stack_.push_overlay(overlay);
	}

	void Application::run() {
		while (running_) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : layer_stack_) {
				layer->on_update();
			}

			window_->on_update();
		}
	};

	bool Application::on_window_closed(WindowClosedEvent& e) {
		running_ = false;
		return true;
	}

}