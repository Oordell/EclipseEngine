#pragma once

#include "eclipse/Core.h"

#include <string>

namespace eclipse {

	enum class EventType {
		None,

		// Window
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,

		// App
		AppTick,
		AppUpdate,
		AppRendere,

		// Keys
		KeyPressed,
		KeyReleased,

		// Mouse
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled
	};

	enum EventCategory : int {
		None = 0,
		Application = BIT(0),
		Input = BIT(1),
		Keyboard = BIT(1),
		Mouse = BIT(1),
		MouseButton = BIT(1)
	};

#define EVENT_CLASS_TYPE(type) static EventType get_static_type() { return EventType::##type; }\
								virtual EventType get_event_type() const override { return get_static_type(); }\
								virtual const char* get_name() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int get_category_flags() const override { return category; }

	class ECLIPSE_API Event {
	public:
		virtual ~Event() = default;

		virtual EventType get_event_type() const = 0;
		virtual const char* get_name() const = 0;
		virtual int get_category_flags() const = 0;
		virtual std::string to_string() const { return get_name(); }

		inline bool is_in_category(EventCategory category) {
			return static_cast<bool>(get_category_flags() & category);
		}

	protected:
		bool handled{ false };
	};

	class ECLIPSE_API EventDispatcher {
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event) : event_(event) {};

		template<typename T>
		bool dispatch(EventFn<T> func) {
			if (event_.get_event_type() != T::get_static_type()) {
				return false;
			}
			event_.handled = func(*(T*)&event_);
			return true;
		}

	private:
		Event& event_;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e) {
		return os << e.to_string();
	}
}