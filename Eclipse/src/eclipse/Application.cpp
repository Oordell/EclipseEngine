#include "Application.h"

#include "Log.h"
#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "Events/WindowEvents.h"
#include "Events/ApplicationEvents.h"

namespace eclipse {

	Application::Application() {};
	Application::~Application() {};

	void Application::run() {
		KeyPressedEvent e1({ 3, 222 });
		KeyReleasedEvent e2(42);
		
		MouseMovedEvent m1({ .x = 2.2F, .y = 3.0F });
		MouseScrolledEvent m2({.x = 0.0F, .y=-10000.9F});
		MouseButtonPressedEvent m3(1);
		MouseButtonReleasedEvent m4(-99);

		WindowResizeEvent w1({ 10, 16 });
		WindowClosedEvent w2;

		AppTickEvent a1;
		AppUpdateEvent a2;
		AppRendereEvent a3;

		EC_DEBUG(e1);
		EC_DEBUG(e2);

		EC_DEBUG(m1);
		EC_DEBUG(m2);
		EC_DEBUG(m3);
		EC_DEBUG(m4);

		EC_DEBUG(w1);
		EC_DEBUG(w2);

		EC_DEBUG(a1);
		EC_DEBUG(a2);
		EC_DEBUG(a3);

		while (true);
	};

}