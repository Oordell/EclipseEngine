#include "ecpch.h"
#include "Application.h"

#include <GLFW/glfw3.h>

namespace eclipse {

	Application::Application() {};

	Application::~Application() {};

	void Application::run() {
		while (running_) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			window_->on_update();
		}
	};

}