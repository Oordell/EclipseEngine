#pragma once

#include "event.h"
#include "eclipse/core/mouse_button_codes.h"
#include "eclipse/common_types/point_2d.h"
#include "eclipse/common_types/mouse_scroll_offset.h"

namespace eclipse {

class ECLIPSE_API MouseMovedEvent : public Event {
public:
	MouseMovedEvent(const Point2D& current_position) : pose_(current_position) {};

	inline au::QuantityF<units::Pixels> get_x() const { return pose_.x; }

	inline au::QuantityF<units::Pixels> get_y() const { return pose_.y; }

	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	EVENT_CLASS_TYPE(MouseMoved)

	std::string to_string() const override {
		std::stringstream ss;
		ss << "MouseMovedEvent. (x, y): (" << pose_.x << ", " << pose_.y << ")";
		return ss.str();
	}

private:
	Point2D pose_ {};
};

class ECLIPSE_API MouseScrolledEvent : public Event {
public:
	MouseScrolledEvent(const MouseScrollOffset& offset) : offset_(offset) {}

	inline au::QuantityF<units::Pixels> get_x_offset() const { return offset_.x; }

	inline au::QuantityF<units::Pixels> get_y_offset() const { return offset_.y; }

	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	EVENT_CLASS_TYPE(MouseScrolled)

	std::string to_string() const override {
		std::stringstream ss;
		ss << "MouseScrolledEvent. Offset (x, y): (" << offset_.x << ", " << offset_.y << ")";
		return ss.str();
	}

private:
	MouseScrollOffset offset_ {};
};

class ECLIPSE_API MouseButtonEvent : public Event {
public:
	inline MouseCode get_mouse_button() const { return button; }

	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)

protected:
	explicit MouseButtonEvent(MouseCode button) : button(button) {}

	MouseCode button = MouseCode::none;
};

class ECLIPSE_API MouseButtonPressedEvent : public MouseButtonEvent {
public:
	MouseButtonPressedEvent(MouseCode button) : MouseButtonEvent(button) {}

	EVENT_CLASS_TYPE(MouseButtonPressed)

	std::string to_string() const override {
		std::stringstream ss;
		ss << "MouseButtonPressedEvent. button: " << button;
		return ss.str();
	}
};

class ECLIPSE_API MouseButtonReleasedEvent : public MouseButtonEvent {
public:
	MouseButtonReleasedEvent(MouseCode button) : MouseButtonEvent(button) {}

	EVENT_CLASS_TYPE(MouseButtonReleased)

	std::string to_string() const override {
		std::stringstream ss;
		ss << "MouseButtonReleasedEvent. button: " << button;
		return ss.str();
	}
};

}  // namespace eclipse