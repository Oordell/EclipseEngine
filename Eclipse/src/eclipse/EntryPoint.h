#pragma once

#ifdef EC_PLATFORM_WINDOWS

extern eclipse::Application* eclipse::create_application();

int main(int argc, char** argv) {
	eclipse::Log::init();
	EC_CORE_WARN("Initialized Log!");

	EC_TRACE("Trace");
	EC_DEBUG("Debug");
	EC_INFO("Info");
	EC_WARN("Warn");
	EC_ERROR("Error");
	EC_FATAL("Fatal");

	auto app = eclipse::create_application();
	app->run();
	delete app;
}

#else
#error Eclipse only supports Windows!
#endif