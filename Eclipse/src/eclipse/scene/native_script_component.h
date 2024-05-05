#pragma once

#include "eclipse/core/core.h"
#include "eclipse/scene/scriptable_entity.h"

namespace eclipse::component {

template <typename T>
concept ScriptableType = std::derived_from<T, ScriptableEntity>;

struct NativeScript {
	ref<ScriptableEntity> instance = nullptr;
	ref<ScriptableEntity> (*instantiate_script_func)();
	void (*destroy_script_func)(NativeScript*);

	template <ScriptableType T>
	void bind() {
		instantiate_script_func = []() { return static_cast<ref<ScriptableEntity>>(make_ref<T>()); };
		/* clang-format off */
		destroy_script_func     = [](NativeScript* ns) {
			ns->instance.reset();
			ns->instance = nullptr;
		};
		/* clang-format on */
	}
};

}  // namespace eclipse::component