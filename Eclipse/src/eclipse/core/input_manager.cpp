#include "ecpch.h"
#include "eclipse/core/input_manager.h"

#ifdef EC_PLATFORM_WINDOWS
#include "platform/windows/windows_input.h"
#endif

namespace eclipse {

scope<InputManager> InputManager::instance_ = InputManager::create();

scope<InputManager> InputManager::create() {
#ifdef EC_PLATFORM_WINDOWS
	return make_scope<WindowsInput>();
#else
	EC_CORE_ASSERT(false, "Unknown platform!");
	return nullptr;
#endif
}

}  // namespace eclipse