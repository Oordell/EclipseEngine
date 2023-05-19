#include "ecpch.h"
#include "imgui_layer.h"

#include "imgui.h"

#include "backends/imgui_impl_glfw.cpp"
#include "backends/imgui_impl_opengl3.cpp"

#include "eclipse/application.h"

namespace eclipse {

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

	ImGuiLayer::~ImGuiLayer() {}

	void ImGuiLayer::on_attach() {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void) io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable keyboard control
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable gamepad control
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// Enable docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// Enable multi-viewport / Platform windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcon;	
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;	

		//Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look imndentical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0F;
			style.Colors[ImGuiCol_WindowBg].w = 1.0F;
		}

		Application& app = Application::get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.get_window().get_native_window());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::on_detach() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::begin() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::end() {
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::get();
		io.DisplaySize = ImVec2(static_cast<float>(app.get_window().get_width()), static_cast<float>(app.get_window().get_height()));

		//Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::on_imgui_render() {
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

}