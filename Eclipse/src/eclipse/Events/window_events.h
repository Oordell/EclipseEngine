#pragma once

#include "eclipse/events/event.h"
#include "eclipse/common_types/window_size.h"

namespace eclipse {

class ECLIPSE_API WindowResizeEvent : public Event {
public:
	WindowResizeEvent(const WindowSize& new_size) : size_(new_size) {}

	inline uint32_t get_width() const { return size_.width; }

	inline uint32_t get_height() const { return size_.height; }

	inline const WindowSize& get_window_size() const { return size_; }

	std::string to_string() const override {
		std::stringstream ss;
		ss << "WindowResizeEvent. width: " << size_.width << ", height: " << size_.height << "";
		return ss.str();
	}

	EVENT_CLASS_TYPE(WindowResize)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)

private:
	WindowSize size_ {};
};

class ECLIPSE_API WindowClosedEvent : public Event {
public:
	WindowClosedEvent() {}

	EVENT_CLASS_TYPE(WindowClose)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

}  // namespace eclipse