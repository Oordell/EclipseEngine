#pragma once

#include "event.h"

namespace eclipse {

	class ECLIPSE_API KeyEvent : public Event {
	public:
		KeyEvent() = delete;
		virtual ~KeyEvent() override = default;

		inline int get_key_code() const { return key_code; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		explicit KeyEvent(int keycode) : key_code(keycode) {}

		int key_code;
	};

	struct KeyPressedArgs {
		int key_code = -1;
		int repeat_count = -1;
	};

	class ECLIPSE_API KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(const KeyPressedArgs& args) : KeyEvent(args.key_code), repeat_count_(args.repeat_count) {};
		~KeyPressedEvent() override = default;

		inline int get_repeate_count() const { return repeat_count_; }

		EVENT_CLASS_TYPE(KeyPressed)

		std::string to_string() const override {
			std::stringstream ss;
			ss << "KeyPressedEvent. Code: " << key_code << ", (" << repeat_count_ << " repeats)";
			return ss.str();
		}

	private:
		int repeat_count_ = 0;
	};

	class ECLIPSE_API KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}
		~KeyReleasedEvent() override = default;

		EVENT_CLASS_TYPE(KeyReleased)

		std::string to_string() const override {
			std::stringstream ss;
			ss << "KeyReleasedEvent. Code: " << key_code;
			return ss.str();
		}
	};

	class ECLIPSE_API KeyTypedEvent : public KeyEvent {
	public:
		KeyTypedEvent(int keycode) : KeyEvent(keycode) {}
		~KeyTypedEvent() override = default;

		EVENT_CLASS_TYPE(KeyTyped)

		std::string to_string() const override {
			std::stringstream ss;
			ss << "KeyTypedEvent. Code: " << key_code;
			return ss.str();
		}
	};

}