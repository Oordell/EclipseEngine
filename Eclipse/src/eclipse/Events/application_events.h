#pragma once

#include "eclipse/events/event.h"

namespace eclipse {

	class ECLIPSE_API AppTickEvent : public Event {
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class ECLIPSE_API AppUpdateEvent : public Event {
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class ECLIPSE_API AppRendereEvent : public Event {
	public:
		AppRendereEvent() {}

		EVENT_CLASS_TYPE(AppRendere)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}