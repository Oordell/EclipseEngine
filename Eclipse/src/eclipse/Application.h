#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"

#include <memory>

namespace eclipse {

	class ECLIPSE_API Application {
	public:
		Application();
		virtual ~Application();

		void run();

	private:
		std::unique_ptr<Window> window_ = std::unique_ptr<Window>(Window::create());
		bool running_ = true;
	};

	// To be defined in client
	Application* create_application();

}