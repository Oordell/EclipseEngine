#include "ecpch.h"
#include "eclipse/core/window.h"

#ifdef EC_PLATFORM_WINDOWS
#include "platform/windows/windows_window.h"
#endif

namespace eclipse {

scope<Window> Window::create(const WindowProps& props) {
#ifdef EC_PLATFORM_WINDOWS
	return make_scope<WindowsWindow>(props);
#else
	EC_CORE_ASSERT(false, "Unknown platform!");
	return nullptr;
#endif
}

}  // namespace eclipse