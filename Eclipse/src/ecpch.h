#pragma once

#include "eclipse/core/platform_detection.h"

/* clang-format off */
#ifdef EC_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
		#define NOMINMAX
	#endif
#endif
/* clang-format on */

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>

#include <numbers>
#include <concepts>

#include "eclipse/core/log.h"
#include "eclipse/debug/instrumentor.h"

#ifdef EC_PLATFORM_WINDOWS
#include <Windows.h>
#endif