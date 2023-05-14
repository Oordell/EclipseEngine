#include "ecpch.h"
#include "imgui_layer.h"

#include "imgui.h"
#include "platform/opengl/imgui_opengl_renderer.h"
#include "eclipse/application.h"

// TEMP!!!
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace eclipse {

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {
		 
	}

	ImGuiLayer::~ImGuiLayer() {

	}

	void ImGuiLayer::on_attach() {
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		// TEMPORARY: should eventually use Eclipse key codes
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::on_detach() {

	}

	void ImGuiLayer::on_update() {
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::get();
		io.DisplaySize = ImVec2(app.get_window().get_width(), app.get_window().get_height());

		float time = static_cast<float>(glfwGetTime());
		io.DeltaTime = time_ > 0.0F ? (time - time_) : (1.0F / 60.0F);
		time_ = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::on_event(Event& event) {
		EventDispatcher dispatcher(event);
		dispatcher.dispatch<MouseButtonPressedEvent>(EC_BIND_EVENT_FN(ImGuiLayer::on_mouse_button_pressed_event));
		dispatcher.dispatch<MouseButtonReleasedEvent>(EC_BIND_EVENT_FN(ImGuiLayer::on_mouse_button_released_event));
		dispatcher.dispatch<MouseMovedEvent>(EC_BIND_EVENT_FN(ImGuiLayer::on_mouse_moved_event));
		dispatcher.dispatch<MouseScrolledEvent>(EC_BIND_EVENT_FN(ImGuiLayer::on_mouse_scrolled_event));
		dispatcher.dispatch<KeyPressedEvent>(EC_BIND_EVENT_FN(ImGuiLayer::on_key_pressed_event));
		dispatcher.dispatch<KeyReleasedEvent>(EC_BIND_EVENT_FN(ImGuiLayer::on_key_released_event));
		dispatcher.dispatch<KeyTypedEvent>(EC_BIND_EVENT_FN(ImGuiLayer::on_key_typed_event));
		dispatcher.dispatch<WindowResizeEvent>(EC_BIND_EVENT_FN(ImGuiLayer::on_window_resize_event));
	}

	bool ImGuiLayer::on_mouse_button_pressed_event(MouseButtonPressedEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.get_mouse_button()] = true;

		return false;
	}

	bool ImGuiLayer::on_mouse_button_released_event(MouseButtonReleasedEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.get_mouse_button()] = false;

		return false;
	}

	bool ImGuiLayer::on_mouse_moved_event(MouseMovedEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.get_x(), e.get_y());

		return false;
	}

	bool ImGuiLayer::on_mouse_scrolled_event(MouseScrolledEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += e.get_y_offset();
		io.MouseWheelH += e.get_x_offset();

		return false;
	}

	bool ImGuiLayer::on_key_pressed_event(KeyPressedEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.get_key_code()] = true;

		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

		return false;
	}

	bool ImGuiLayer::on_key_released_event(KeyReleasedEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.get_key_code()] = false;

		return false;
	}

	bool ImGuiLayer::on_key_typed_event(KeyTypedEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		int key_code = e.get_key_code();
		if (key_code > 0 && key_code < 0x10000) {
			io.AddInputCharacter(static_cast<unsigned short>(key_code));
		}

		return false;
	}

	bool ImGuiLayer::on_window_resize_event(WindowResizeEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(e.get_width(), e.get_height());
		io.DisplayFramebufferScale = ImVec2(1.0F, 1.0F);
		glViewport(0, 0, e.get_width(), e.get_height());

		return false;
	}

}