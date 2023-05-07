#pragma once

#include "eclipse/Events/Event.h"

#include <sstream>

namespace eclipse {

	class ECLIPSE_API AppTickEvent : public Event {
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(Application)
	};

	class ECLIPSE_API AppUpdateEvent : public Event {
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(Application)
	};

	class ECLIPSE_API AppRendereEvent : public Event {
	public:
		AppRendereEvent() {}

		EVENT_CLASS_TYPE(AppRendere)
		EVENT_CLASS_CATEGORY(Application)
	};
}