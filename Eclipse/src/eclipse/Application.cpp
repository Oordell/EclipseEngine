#include "ecpch.h"
#include "application.h"

#include <glad/glad.h>

#include "input_manager.h"

namespace eclipse {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::instance_ = nullptr;

	Application::Application() {
		EC_CORE_ASSERT(!instance_, "Application already exists!");
		instance_ = this;
		window_->set_event_callback(BIND_EVENT_FN(on_event));

		push_overlay(imgui_layer_.get());

		glGenVertexArrays(1, &vertex_array_);
		glBindVertexArray(vertex_array_);

		static const int dimentions = 3;
		float vertices[dimentions * dimentions] = {
			-0.9F, -0.9F, 0.0F,
			0.9F, -0.9F, 0.0F,
			0.0F, 0.9F, 0.0F
		};

		vertex_buffer_.reset(VertexBuffer::create(vertices, sizeof(vertices)));

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, dimentions * sizeof(float), nullptr);
		uint32_t indices[dimentions] = {0, 1, 2};
		index_buffer_.reset(IndexBuffer::create(indices, dimentions));

		std::string vertex_src = R"(
			#version 330 core
			
			layout(location = 0) in vec3 position_;

			out vec3 v_position;
			
			void main() {
				v_position = position_;
				gl_Position = vec4(position_, 1.0);
			}
		)";

		std::string fragments_src = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_position;
			
			void main() {
				color = vec4(v_position * 0.5 + 0.5, 1.0);
			}
		)";

		shader_ = std::make_unique<Shader>(vertex_src, fragments_src);
	};

	Application::~Application() {};

	void Application::on_event(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowClosedEvent>(BIND_EVENT_FN(on_window_closed));

		//EC_CORE_INFO("{0}", e);

		for (auto it = layer_stack_.end(); it != layer_stack_.begin(); ) {
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
			static const float red = 0.1F;
			static const float green = 0.1F;
			static const float blue = 0.1F;
			static const float alpha = 1.0F;
			glClearColor(red, green, blue, alpha);
			glClear(GL_COLOR_BUFFER_BIT);

			shader_->bind();
			glBindVertexArray(vertex_array_);
			glDrawElements(GL_TRIANGLES, index_buffer_->get_count(), GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : layer_stack_) {
				layer->on_update();
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

}