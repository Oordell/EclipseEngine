#include "ecpch.h"
#include "application.h"
#include "eclipse/renderer/renderer.h"
#include "input_manager.h"

#include <GLFW/glfw3.h>

namespace eclipse {

Application* Application::instance_ = nullptr;

Application::Application(const std::string& name) {
	EC_PROFILE_FUNCTION();
	EC_CORE_ASSERT(!instance_, "Application already exists!");
	instance_ = this;
	window_   = Window::create({.title = name});
	window_->set_event_callback(EC_BIND_EVENT_FN(Application::on_event));

	Renderer::init();

	push_overlay(imgui_layer_.get());
};

Application::~Application() {
	EC_PROFILE_FUNCTION();

	Renderer::shutdown();
}

void Application::close() { running_ = false; };

void Application::on_event(Event& e) {
	EC_PROFILE_FUNCTION();

	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowClosedEvent>(EC_BIND_EVENT_FN(Application::on_window_closed));
	dispatcher.dispatch<WindowResizeEvent>(EC_BIND_EVENT_FN(Application::on_window_resize));

	for (auto it = layer_stack_.rbegin(); it != layer_stack_.rend(); ++it) {
		if (e.handled) {
			break;
		}
		(*it)->on_event(e);
	}
}

void Application::push_layer(Layer* layer) {
	EC_PROFILE_FUNCTION();

	layer_stack_.push_layer(layer);
	layer->on_attach();
}

void Application::push_overlay(Layer* overlay) {
	EC_PROFILE_FUNCTION();

	layer_stack_.push_overlay(overlay);
	overlay->on_attach();
}

void Application::run() {
	EC_PROFILE_FUNCTION();

	while (running_) {
		EC_PROFILE_SCOPE("**Application Run Loop**");

		auto time         = static_cast<float>(glfwGetTime());
		Timestep timestep = Timestep(time - last_frame_time_);
		last_frame_time_  = time;

		if (!minimized_) {
			{
				EC_PROFILE_SCOPE("LayerStack calling on_update()");
				for (Layer* layer : layer_stack_) {
					layer->on_update(timestep);
				}
			}

			{
				EC_PROFILE_SCOPE("LayerStack calling on_imgui_render()");
				imgui_layer_->begin();
				for (Layer* layer : layer_stack_) {
					layer->on_imgui_render();
				}
				imgui_layer_->end();
			}
		}

		window_->on_update();
	}
};

bool Application::on_window_closed(WindowClosedEvent& e) {
	running_ = false;
	return true;
}

bool Application::on_window_resize(WindowResizeEvent& e) {
	EC_PROFILE_FUNCTION();

	if (e.get_width() == 0 || e.get_height() == 0) {
		minimized_ = true;
		return false;
	}
	minimized_ = false;
	Renderer::on_window_resize(e.get_window_size());

	return false;
}

}  // namespace eclipse