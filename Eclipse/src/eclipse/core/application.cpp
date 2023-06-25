#include "ecpch.h"
#include "application.h"
#include "eclipse/renderer/renderer.h"
#include "input_manager.h"

#include <GLFW/glfw3.h>

namespace eclipse {

Application* Application::instance_ = nullptr;

Application::Application() {
	EC_CORE_ASSERT(!instance_, "Application already exists!");
	instance_ = this;
	window_->set_event_callback(EC_BIND_EVENT_FN(Application::on_event));

	Renderer::init();

	push_overlay(imgui_layer_.get());
};

Application::~Application() { Renderer::shutdown(); };

void Application::on_event(Event& e) {
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowClosedEvent>(EC_BIND_EVENT_FN(Application::on_window_closed));
	dispatcher.dispatch<WindowResizeEvent>(EC_BIND_EVENT_FN(Application::on_window_resize));

	//	EC_CORE_INFO("{0}", e);

	for (auto it = layer_stack_.end(); it != layer_stack_.begin();) {
		(*--it)->on_event(e);
		if (e.handled) {
			break;
		}
	}
}

void Application::push_layer(Layer* layer) {
	layer_stack_.push_layer(layer);
	layer->on_attach();
}

void Application::push_overlay(Layer* overlay) {
	layer_stack_.push_overlay(overlay);
	overlay->on_attach();
}

void Application::run() {
	while (running_) {
		auto time         = static_cast<float>(glfwGetTime());
		Timestep timestep = Timestep(time - last_frame_time_);
		last_frame_time_  = time;

		if (!minimized_) {
			for (Layer* layer : layer_stack_) {
				layer->on_update(timestep);
			}
		}

		imgui_layer_->begin();
		for (Layer* layer : layer_stack_) {
			layer->on_imgui_render();
		}
		imgui_layer_->end();

		window_->on_update();
	}
};

bool Application::on_window_closed(WindowClosedEvent& e) {
	running_ = false;
	return true;
}

bool Application::on_window_resize(WindowResizeEvent& e) {
	if (e.get_width() == 0 || e.get_height() == 0) {
		minimized_ = true;
		return false;
	}
	minimized_ = false;
	Renderer::on_window_resize(e.get_window_size());

	return false;
}

}  // namespace eclipse