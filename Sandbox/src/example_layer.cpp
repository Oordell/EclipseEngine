#include "example_layer.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

ExampleLayer::ExampleLayer() : Layer("Example") {
	static const int dimentions = 3;

	float vertices[3 * 7] = {
	    /* clang-format off */
			-0.5F, -0.5F, 0.0F, 1.0F, 0.0F, 1.0F, 1.0F, 
			0.5F, -0.5F, 0.0F, 0.0F, 0.0F,  1.0F, 1.0F, 
			0.0F, 0.5F, 0.0F, 1.0F, 1.0F, 0.0F,  1.0F
	    /* clang-format on */
	};

	auto vertex_buffer = eclipse::VertexBuffer::create(vertices, sizeof(vertices));

	eclipse::BufferLayout layout {{eclipse::ShaderDataType::floatvec3, "position_"},
	                              {eclipse::ShaderDataType::floatvec4, "color_"}};
	vertex_buffer->set_layout(layout);
	vertex_array_->add_vertex_buffer(vertex_buffer);

	uint32_t indices[dimentions] = {0, 1, 2};
	auto index_buffer            = eclipse::IndexBuffer::create(indices, dimentions);
	vertex_array_->set_index_buffer(index_buffer);

	float square_vertices[5 * 4] = {
	    /* clang-format off */
			-0.5F, -0.5F, 0.0F, 0.0F, 0.0F,
			0.5F, -0.5F, 0.0F, 1.0F, 0.0F,
			0.5F, 0.5F, 0.0F, 1.0F, 1.0F,
			-0.5F, 0.5F, 0.0F, 0.0F, 1.0F
	    /* clang-format on */
	};

	auto square_vertex_buffer = eclipse::VertexBuffer::create(square_vertices, sizeof(square_vertices));
	square_vertex_buffer->set_layout(
	    {{eclipse::ShaderDataType::floatvec3, "position_"}, {eclipse::ShaderDataType::floatvec2, "tex_coord_"}});
	square_vertex_array_->add_vertex_buffer(square_vertex_buffer);

	uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};
	auto square_index_buffer_  = eclipse::IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t));
	square_vertex_array_->set_index_buffer(square_index_buffer_);

	std::string vertex_src = R"(
			#version 330 core
			
			layout(location = 0) in vec3 position_;
			layout(location = 1) in vec4 color_;

			uniform mat4 view_projection;
			uniform mat4 transform;

			out vec3 v_position;
			out vec4 v_color;
			
			void main() {
				v_position = position_;
				v_color = color_;
				gl_Position = view_projection * transform * vec4(position_, 1.0);
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

	shader_ = eclipse::Shader::create({.name = "VertexPosColor", .vertex_src = vertex_src, .fragment_src = fragments_src});

	std::string flat_color_vertex_src = R"(
			#version 330 core
			
			layout(location = 0) in vec3 position_;

			uniform mat4 view_projection;
			uniform mat4 transform;

			out vec3 v_position;
			
			void main() {
				v_position = position_;
				gl_Position = view_projection * transform * vec4(position_, 1.0);
			}
		)";

	std::string flat_color_fragments_src = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_position;

			uniform vec3 u_color;
			
			void main() {
				color = vec4(u_color, 1.0);
			}
		)";

	flat_color_shader_ = eclipse::Shader::create(
	    {.name = "FlatColor", .vertex_src = flat_color_vertex_src, .fragment_src = flat_color_fragments_src});

	auto texture_shader = shader_library_.load(std::filesystem::path("assets/shaders/texture.glsl"));

	texture_shader->bind();
	texture_shader->set_int("u_texture", 0);
}

void ExampleLayer::on_update(au::QuantityF<au::Seconds> timestep) {
	camera_controller_.on_update(timestep);

	frame_rate_ = static_cast<unsigned int>(1.0F / timestep.in(au::seconds));
	EC_TRACE_THROTTLED(0.25, "Frame rate: {0}Hz", frame_rate_);

	static const float red   = 0.1F;
	static const float green = 0.1F;
	static const float blue  = 0.1F;
	static const float alpha = 1.0F;

	eclipse::RenderCommand::set_clear_color({red, green, blue, alpha});
	eclipse::RenderCommand::clear();

	eclipse::Renderer::begin_scene(camera_controller_.get_camera());

	static const glm::mat4 identity_matrix = glm::mat4(1.0F);
	static const glm::mat4 scale           = glm::scale(identity_matrix, glm::vec3(0.1F));
	static const unsigned int no_tiles     = 20;
	static const float tile_position_scale = 0.11F;

	flat_color_shader_->bind();
	flat_color_shader_->set_float3("u_color", square_color_);

	for (int y = 0; y < no_tiles; y++) {
		for (int x = 0; x < no_tiles; x++) {
			glm::vec3 position(x * tile_position_scale, y * tile_position_scale, 0.0F);
			glm::mat4 square_transform = glm::translate(identity_matrix, position) * scale;
			eclipse::Renderer::submit(flat_color_shader_, square_vertex_array_, square_transform);
		}
	}

	auto texture_shader = shader_library_.get("texture");

	texture_->bind(0);
	eclipse::Renderer::submit(texture_shader, square_vertex_array_, glm::scale(identity_matrix, glm::vec3(1.5F)));
	texture_ordell_logo_->bind(0);
	eclipse::Renderer::submit(texture_shader, square_vertex_array_, glm::scale(identity_matrix, glm::vec3(1.5F)));

	// Triangle
	//	eclipse::Renderer::submit(shader_, vertex_array_);

	eclipse::Renderer::end_scene();
}

void ExampleLayer::on_imgui_render() {
	std::string text = "Frame rate: " + std::to_string(frame_rate_);
	ImGui::Begin("Test");
	ImGui::Text(text.c_str());
	ImGui::End();

	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square color", glm::value_ptr(square_color_));
	ImGui::End();
}

void ExampleLayer::on_event(eclipse::Event& event) {
	camera_controller_.on_event(event);
	eclipse::EventDispatcher dispatcher(event);
	dispatcher.dispatch<eclipse::KeyPressedEvent>(EC_BIND_EVENT_FN(ExampleLayer::on_key_pressed_event));
}