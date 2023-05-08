#include <Eclipse.h>

class Sandbox : public eclipse::Application {
public:
	Sandbox() {}
	~Sandbox() {}
};

eclipse::Application *eclipse::create_application() {
	return new Sandbox();
}