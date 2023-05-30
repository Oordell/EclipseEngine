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

	static const int dimentions = 3;
	float vertices[3 * 7]       = {
		-0.9F, -0.9F, 0.0F, 1.0F, 0.0F, 1.0F, 1.0F, 
		0.9F, -0.9F, 0.0F, 0.0F, 0.0F,  1.0F, 1.0F, 
		0.0F, 0.9F, 0.0F, 1.0F, 1.0F, 0.0F,  1.0F
	};

	std::shared_ptr<VertexBuffer> vertex_buffer;
	vertex_buffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));

	BufferLayout layout {{ShaderDataType::floatvec3, "position_"}, {ShaderDataType::floatvec4, "color_"}};
	vertex_buffer->set_layout(layout);
	vertex_array_->add_vertex_buffer(vertex_buffer);

	uint32_t indices[dimentions] = {0, 1, 2};
	std::shared_ptr<IndexBuffer> index_buffer;
	index_buffer.reset(IndexBuffer::create(indices, dimentions));
	vertex_array_->set_index_buffer(index_buffer);

	float square_vertices[3 * 4] = {
		-0.5F, -0.5F, 0.0F, 
		0.5F, -0.5F, 0.0F, 
		0.5F, 0.5F, 0.0F, 
		-0.5F, 0.5F, 0.0F};

	std::shared_ptr<VertexBuffer> square_vertex_buffer;
	square_vertex_buffer.reset(VertexBuffer::create(square_vertices, sizeof(square_vertices)));
	square_vertex_buffer->set_layout({{ShaderDataType::floatvec3, "position_"}});
	square_vertex_array_->add_vertex_buffer(square_vertex_buffer);

	uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};
	std::shared_ptr<IndexBuffer> square_index_buffer_;
	square_index_buffer_.reset(IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t)));
	square_vertex_array_->set_index_buffer(square_index_buffer_);

	std::string vertex_src = R"(
			#version 330 core
			
			layout(location = 0) in vec3 position_;
			layout(location = 1) in vec4 color_;

			out vec3 v_position;
			out vec4 v_color;
			
			void main() {
				v_position = position_;
				v_color = color_;
				gl_Position = vec4(position_, 1.0);
			}
		)";

	std::string fragments_src = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_position;
			in vec4 v_color;
			
			void main() {
				color = vec4(v_position * 0.5 + 0.5, 1.0);
				color = v_color;
			}
		)";

	shader_ = std::make_unique<Shader>(vertex_src, fragments_src);

	std::string blue_vertex_src = R"(
			#version 330 core
			
			layout(location = 0) in vec3 position_;

			out vec3 v_position;
			
			void main() {
				v_position = position_;
				gl_Position = vec4(position_, 1.0);
			}
		)";

	std::string blue_fragments_src = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_position;
			
			void main() {
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

	blue_shader_ = std::make_unique<Shader>(blue_vertex_src, blue_fragments_src);
};

Application::~Application() {};

void Application::on_event(Event& e) {
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<WindowClosedEvent>(BIND_EVENT_FN(on_window_closed));

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
		static const float red   = 0.1F;
		static const float green = 0.1F;
		static const float blue  = 0.1F;
		static const float alpha = 1.0F;
		glClearColor(red, green, blue, alpha);
		glClear(GL_COLOR_BUFFER_BIT);

		shader_->bind();
		vertex_array_->bind();
		glDrawElements(GL_TRIANGLES, vertex_array_->get_index_buffer()->get_count(), GL_UNSIGNED_INT, nullptr);
		
		blue_shader_->bind();
		square_vertex_array_->bind();
		glDrawElements(GL_TRIANGLES, square_vertex_array_->get_index_buffer()->get_count(), GL_UNSIGNED_INT, nullptr);

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

}  // namespace eclipse