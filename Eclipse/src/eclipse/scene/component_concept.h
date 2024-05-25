#pragma once
#include "eclipse/scene/components.h"
#include "eclipse/scene/native_script_component.h"

#include <concepts>

namespace eclipse::component {

template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template <typename T>
concept IsComponent = IsAnyOf<T, ID, Tag, Transform, Color, SpriteRenderer, CircleRenderer, Camera, RigidBody2D,
                              BoxCollider2D, CircleCollider2D, NativeScript>;

template <IsComponent... Component>
struct ComponentPack {};

using AllComponents = ComponentPack<ID, Tag, Transform, Color, SpriteRenderer, CircleRenderer, Camera, RigidBody2D,
                                    BoxCollider2D, CircleCollider2D, NativeScript>;

}  // namespace eclipse::component