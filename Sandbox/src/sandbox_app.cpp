#include <eclipse.h>

#include "imgui/imgui.h"

class ExampleLayer : public eclipse::Layer {
public:
	ExampleLayer() : Layer("Example") {
		static const int dimentions = 3;
		float vertices[3 * 7]       = {
      /* clang-format off */
			-0.5F, -0.5F, 0.0F, 1.0F, 0.0F, 1.0F, 1.0F, 
			0.5F, -0.5F, 0.0F, 0.0F, 0.0F,  1.0F, 1.0F, 
			0.0F, 0.5F, 0.0F, 1.0F, 1.0F, 0.0F,  1.0F
      /* clang-format on */
  };

		std::shared_ptr<eclipse::VertexBuffer> vertex_buffer;
		vertex_buffer.reset(eclipse::VertexBuffer::create(vertices, sizeof(vertices)));

		eclipse::BufferLayout layout {{eclipse::ShaderDataType::floatvec3, "position_"},
		                              {eclipse::ShaderDataType::floatvec4, "color_"}};
		vertex_buffer->set_layout(layout);
		vertex_array_->add_vertex_buffer(vertex_buffer);

		uint32_t indices[dimentions] = {0, 1, 2};
		std::shared_ptr<eclipse::IndexBuffer> index_buffer;
		index_buffer.reset(eclipse::IndexBuffer::create(indices, dimentions));
		vertex_array_->set_index_buffer(index_buffer);

		float square_vertices[3 * 4] = {
		    /* clang-format off */
			-0.75F, -0.75F, 0.0F, 
			0.75F, -0.75F, 0.0F, 
			0.75F, 0.75F, 0.0F, 
			-0.75F, 0.75F, 0.0F
		    /* clang-format on */
		};

		std::shared_ptr<eclipse::VertexBuffer> square_vertex_buffer;
		square_vertex_buffer.reset(eclipse::VertexBuffer::create(square_vertices, sizeof(square_vertices)));
		square_vertex_buffer->set_layout({{eclipse::ShaderDataType::floatvec3, "position_"}});
		square_vertex_array_->add_vertex_buffer(square_vertex_buffer);

		uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};
		std::shared_ptr<eclipse::IndexBuffer> square_index_buffer_;
		square_index_buffer_.reset(eclipse::IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t)));
		square_vertex_array_->set_index_buffer(square_index_buffer_);

		std::string vertex_src = R"(
			#version 330 core
			
			layout(location = 0) in vec3 position_;
			layout(location = 1) in vec4 color_;

			uniform mat4 view_projection;

			out vec3 v_position;
			out vec4 v_color;
			
			void main() {
				v_position = position_;
				v_color = color_;
				gl_Position = view_projection * vec4(position_, 1.0);
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

		shader_ = std::make_unique<eclipse::Shader>(vertex_src, fragments_src);

		std::string blue_vertex_src = R"(
			#version 330 core
			
			layout(location = 0) in vec3 position_;

			uniform mat4 view_projection;

			out vec3 v_position;
			
			void main() {
				v_position = position_;
				gl_Position = view_projection * vec4(position_, 1.0);
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

		blue_shader_ = std::make_unique<eclipse::Shader>(blue_vertex_src, blue_fragments_src);
	}

	~ExampleLayer() = default;

	void on_update(eclipse::Timestep timestep) override {
		frame_rate_ = static_cast<unsigned int>(1.0F / timestep);
		EC_TRACE_THROTTLED(0.25, "Frame rate: {0}Hz", frame_rate_);

		if (eclipse::InputManager::is_key_pressed(EC_KEY_LEFT)) {
			camera_position_.x -= camera_move_speed_ * timestep;
		} else if (eclipse::InputManager::is_key_pressed(EC_KEY_RIGHT)) {
			camera_position_.x += camera_move_speed_ * timestep;
		}
		if (eclipse::InputManager::is_key_pressed(EC_KEY_UP)) {
			camera_position_.y += camera_move_speed_ * timestep;
		} else if (eclipse::InputManager::is_key_pressed(EC_KEY_DOWN)) {
			camera_position_.y -= camera_move_speed_ * timestep;
		}
		if (eclipse::InputManager::is_key_pressed(EC_KEY_A)) {
			camera_rotation_ += camera_rotate_speed_ * timestep;
		} else if (eclipse::InputManager::is_key_pressed(EC_KEY_D)) {
			camera_rotation_ -= camera_rotate_speed_ * timestep;
		}

		static const float red   = 0.1F;
		static const float green = 0.1F;
		static const float blue  = 0.1F;
		static const float alpha = 1.0F;

		eclipse::RenderCommand::set_clear_color({red, green, blue, alpha});
		eclipse::RenderCommand::clear();

		camera_.set_position(camera_position_);
		camera_.set_rotation(camera_rotation_);

		eclipse::Renderer::begin_scene(camera_);

		eclipse::Renderer::submit(blue_shader_, square_vertex_array_);
		eclipse::Renderer::submit(shader_, vertex_array_);

		eclipse::Renderer::end_scene();
	}

	void on_imgui_render() override {
		std::string text = "Frame rate: " + std::to_string(frame_rate_);
		ImGui::Begin("Test");
		ImGui::Text(text.c_str());
		ImGui::End();
	}

	void on_event(eclipse::Event& event) {
		eclipse::EventDispatcher dispatcher(event);
		dispatcher.dispatch<eclipse::KeyPressedEvent>(EC_BIND_EVENT_FN(ExampleLayer::on_key_pressed_event));
	}

	bool on_key_pressed_event(eclipse::KeyPressedEvent& event) { return false; }

private:
	std::shared_ptr<eclipse::Shader> shader_;
	std::shared_ptr<eclipse::VertexArray> vertex_array_ =
	    std::shared_ptr<eclipse::VertexArray>(eclipse::VertexArray::create());

	std::shared_ptr<eclipse::Shader> blue_shader_;
	std::shared_ptr<eclipse::VertexArray> square_vertex_array_ =
	    std::shared_ptr<eclipse::VertexArray>(eclipse::VertexArray::create());

	glm::vec3 camera_position_ {0.0F, 0.0F, 0.0F};
	float camera_rotation_ {0.0F};
	eclipse::OrthographicCamera camera_ {{.left = -1.6F, .right = 1.6F, .bottom = -0.9F, .top = 0.9F}};
	float camera_move_speed_   = 2.0F;   // [units / sec]
	float camera_rotate_speed_ = 90.0F;  // [degrees / sec]
	unsigned int frame_rate_   = 0;
};

class Sandbox : public eclipse::Application {
public:
	Sandbox() { push_layer(new ExampleLayer()); }

	~Sandbox() {}
};

eclipse::Application* eclipse::create_application() { return new Sandbox(); }