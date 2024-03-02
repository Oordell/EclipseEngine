#pragma once

#include "ecpch.h"

#include "eclipse/core/core.h"
#include "eclipse/events/event.h"
#include "eclipse/common_types/window_size.h"

namespace eclipse {

struct WindowProps {
	std::string title = "Eclipse App";
	WindowSize window_size {.width = units::pixels(1280), .height = units::pixels(720)};
};

class ECLIPSE_API Window {
public:
	using EventCallbackfn = std::function<void(Event&)>;

	virtual ~Window() = default;

	virtual void on_update()                                         = 0;
	virtual au::Quantity<units::Pixels, uint32_t> get_height() const = 0;
	virtual au::Quantity<units::Pixels, uint32_t> get_width() const  = 0;
	virtual void set_event_callback(const EventCallbackfn& callback) = 0;
	virtual void set_v_sync(bool enabled)                            = 0;
	virtual bool is_v_sync() const                                   = 0;
	virtual void* get_native_window() const                          = 0;

	static scope<Window> create(const WindowProps& props = {});
};
}  // namespace eclipse