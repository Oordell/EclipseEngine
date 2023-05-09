#include "ecpch.h"
#include "Application.h"

#include <GLFW/glfw3.h>

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
	}

	void Application::run() {
		while (running_) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			window_->on_update();
		}
	};

	bool Application::on_window_closed(WindowClosedEvent& e) {
		running_ = false;
		return true;
	}

}