#include "ecpch.h"
#include "imgui_layer.h"
#include "font_library.h"

#include "imgui.h"

#include "backends/imgui_impl_glfw.cpp"
#include "backends/imgui_impl_opengl3.cpp"

#include "eclipse/core/application.h"

#include "ImGuizmo.h"

namespace eclipse {

ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::on_attach() {
	EC_PROFILE_FUNCTION();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void) io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard control
	//	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable gamepad control
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable multi-viewport / Platform windows
	//	io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcon;
	//	io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	// Add fonts
	fonts::FontLibrary::add_font(FilePath("assets/fonts/gochi-hand/GochiHand-Regular.ttf"), "GochiHand-Regular", 20.0F);
	fonts::FontLibrary::add_font(FilePath("assets/fonts/rock-salt/RockSalt-Regular.ttf"), "RockSalt-Regular", 22.0F);
	fonts::FontLibrary::add_font(FilePath("assets/fonts/rock-salt/RockSalt-Regular.ttf"), "RockSalt-Regular");
	fonts::FontLibrary::add_font(FilePath("assets/fonts/roboto-mono/static/RobotoMono-Regular.ttf"), "RobotoMono-Regular");
	fonts::FontLibrary::add_font(FilePath("assets/fonts/roboto/Roboto-Regular.ttf"), "Roboto-Regular");
	fonts::FontLibrary::add_font(FilePath("assets/fonts/roboto/Roboto-Italic.ttf"), "Roboto-Italic");
	fonts::FontLibrary::add_font(FilePath("assets/fonts/roboto/Roboto-Bold.ttf"), "Roboto-Bold");
	auto [success, font_data] = fonts::FontLibrary::get_font_index_by_name("Roboto-Regular");
	EC_CORE_ASSERT(success, "Font was not found!");

	io.FontDefault = io.Fonts->Fonts[font_data.index];

	//	Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//	ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look imndentical to regular
	// ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding              = 0.0F;
		style.Colors[ImGuiCol_WindowBg].w = 1.0F;
	}
	set_dark_theme_colors();

	Application& app   = Application::get();
	GLFWwindow* window = static_cast<GLFWwindow*>(app.get_window().get_native_window());

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::on_detach() {
	EC_PROFILE_FUNCTION();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiLayer::on_event(Event& e) {
	if (block_events_) {
		ImGuiIO& io = ImGui::GetIO();
		e.handled |= e.is_in_category(EventCategoryMouse) & io.WantCaptureMouse;
		e.handled |= e.is_in_category(EventCategoryKeyboard) & io.WantCaptureKeyboard;
	}
}

void ImGuiLayer::begin() {
	EC_PROFILE_FUNCTION();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

void ImGuiLayer::end() {
	EC_PROFILE_FUNCTION();

	ImGuiIO& io      = ImGui::GetIO();
	Application& app = Application::get();
	io.DisplaySize =
	    ImVec2(static_cast<float>(app.get_window().get_width()), static_cast<float>(app.get_window().get_height()));

	//	Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void ImGuiLayer::set_dark_theme_colors() {
	auto& colors              = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4 {0.1F, 0.105F, 0.11F, 1.0F};

	// Headers
	colors[ImGuiCol_Header]        = ImVec4 {0.2F, 0.205F, 0.21F, 1.0F};
	colors[ImGuiCol_HeaderHovered] = ImVec4 {0.3F, 0.305F, 0.31F, 1.0F};
	colors[ImGuiCol_HeaderActive]  = ImVec4 {0.15F, 0.1505F, 0.151F, 1.0F};

	// Buttons
	colors[ImGuiCol_Button]        = ImVec4 {0.2F, 0.205F, 0.21F, 1.0F};
	colors[ImGuiCol_ButtonHovered] = ImVec4 {0.3F, 0.305F, 0.31F, 1.0F};
	colors[ImGuiCol_ButtonActive]  = ImVec4 {0.15F, 0.1505F, 0.151F, 1.0F};

	// Frame BG
	colors[ImGuiCol_FrameBg]        = ImVec4 {0.2F, 0.205F, 0.21F, 1.0F};
	colors[ImGuiCol_FrameBgHovered] = ImVec4 {0.3F, 0.305F, 0.31F, 1.0F};
	colors[ImGuiCol_FrameBgActive]  = ImVec4 {0.15F, 0.1505F, 0.151F, 1.0F};

	// Tabs
	colors[ImGuiCol_Tab]                = ImVec4 {0.15F, 0.1505F, 0.151F, 1.0F};
	colors[ImGuiCol_TabHovered]         = ImVec4 {0.38F, 0.3805F, 0.381F, 1.0F};
	colors[ImGuiCol_TabActive]          = ImVec4 {0.28F, 0.2805F, 0.281F, 1.0F};
	colors[ImGuiCol_TabUnfocused]       = ImVec4 {0.15F, 0.1505F, 0.151F, 1.0F};
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4 {0.2F, 0.205F, 0.21F, 1.0F};

	// Title
	colors[ImGuiCol_TitleBg]          = ImVec4 {0.15F, 0.1505F, 0.151F, 1.0F};
	colors[ImGuiCol_TitleBgActive]    = ImVec4 {0.15F, 0.1505F, 0.151F, 1.0F};
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4 {0.15F, 0.1505F, 0.151F, 1.0F};
}

}  // namespace eclipse