#pragma once

#include "eclipse/common_types/file_path.h"
#include "eclipse/core/application.h"

#ifdef EC_PLATFORM_WINDOWS

extern eclipse::Application* eclipse::create_application(const ApplicationCommandLineArgs& args);

int main(int argc, char** argv) {
	eclipse::Log::init();
	EC_CORE_DEBUG("Initialized Log!");

	EC_PROFILE_BEGIN_SESSION("startup", eclipse::FilePath("logs/eclipse_profile_startup.json"));
	auto app = eclipse::create_application({.count = argc, .args = argv});
	EC_PROFILE_END_SESSION();

	EC_PROFILE_BEGIN_SESSION("runtime", eclipse::FilePath("logs/eclipse_profile_runtime.json"));
	app->run();
	EC_PROFILE_END_SESSION();

	EC_PROFILE_BEGIN_SESSION("shutdown", eclipse::FilePath("logs/eclipse_profile_shutdown.json"));
	delete app;
	EC_PROFILE_END_SESSION();
}

#else
#error Eclipse only supports Windows!
#endif