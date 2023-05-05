#pragma once

#ifdef EC_PLATFORM_WINDOWS

extern eclipse::Application* eclipse::create_application();

int main(int argc, char** argv) {
	auto app = eclipse::create_application();
	app->run();
	delete app;
}

#else
#error Eclipse only supports Windows!
#endif