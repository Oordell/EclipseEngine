#pragma once

#include "Core.h"

namespace eclipse {

class ECLIPSE_API Application
{
public:
	Application();
	virtual ~Application();

	void run();
};

// To be defined in client
Application* create_application();

}