#pragma once

#include "eclipse/Events/Event.h"
#include "eclipse/CommonTypes/WindowSize.h"

namespace eclipse {

	class ECLIPSE_API WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(const WindowSize& new_size) : size_(new_size) {}

		inline unsigned int get_width() const { return size_.width; }
		inline unsigned int get_height() const { return size_.height; }

		std::string to_string() const override {
			std::stringstream ss;
			ss << "WindowResizeEvent. width: " << size_.width << ", height: " << size_.height << "";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		WindowSize size_{};
	};

	class ECLIPSE_API WindowClosedEvent : public Event {
	public:
		WindowClosedEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

}