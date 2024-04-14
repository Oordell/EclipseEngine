-- include "dependencies.lua"

workspace "Eclipse"
	architecture "x86_64"
	startproject "Helios"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	flags {
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

VULKAN_SDK = os.getenv("VULKAN_SDK")

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Eclipse/vendor/GLFW_fork/include"
IncludeDir["Glad"] = "%{wks.location}/Eclipse/vendor/GLAD/include"
IncludeDir["ImGui"] = "%{wks.location}/Eclipse/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Eclipse/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Eclipse/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Eclipse/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Eclipse/vendor/yaml-cpp/include"
IncludeDir["imguizmo"] = "%{wks.location}/Eclipse/vendor/imguizmo"
IncludeDir["au"] = "%{wks.location}/Eclipse/vendor/au"
IncludeDir["shaderc"] = "%{wks.location}/Eclipse/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Eclipse/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/Eclipse/vendor/VulkanSDK/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

group "Dependencies"
	include "Eclipse/vendor/GLFW_fork"
	include "Eclipse/vendor/Glad"
	include "Eclipse/vendor/imgui"
	include "Eclipse/vendor/yaml-cpp"
group ""

project "Eclipse"
	location "Eclipse"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "ecpch.h"
	pchsource "%{prj.name}/src/ecpch.cpp"

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/imguizmo/ImGuizmo.h",
		"%{prj.name}/vendor/imguizmo/ImGuizmo.cpp",
		"%{prj.name}/vendor/au/**.hh",
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE"
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.imguizmo}",
		"%{IncludeDir.au}",
		"%{IncludeDir.VulkanSDK}"
	}

	links {
		"GLFW",
		"Glad",
		"ImGui",
		"yaml-cpp",
		"user32.lib",
		"gdi32.lib",
		"opengl32.lib"
	}

	filter "files:Eclipse/vendor/imguizmo/**.cpp"
		flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		defines {
			"EC_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "ECLIPSE_DEBUG"
		runtime "Debug"
		symbols "on"

		links {
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

	filter "configurations:Release"
		defines "ECLIPSE_RELEASE"
		runtime "Release"
		optimize "on"

		links {
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

	filter "configurations:Dist"
		defines "ECLIPSE_DIST"
		runtime "Release"
		optimize "on"

		links {
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	cppdialect "C++20"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	defines {
		"YAML_CPP_STATIC_DEFINE"
	}

	includedirs {
		"Eclipse/vendor/spdlog/include",
		"Eclipse/src",
		"Eclipse/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.au}"
	}

	links {
		"Eclipse"
	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"EC_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "ECLIPSE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ECLIPSE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ECLIPSE_DIST"
		runtime "Release"
		optimize "on"


project "Helios"
	location "Helios"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	cppdialect "C++20"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	defines {
		"YAML_CPP_STATIC_DEFINE"
	}

	includedirs {
		"Eclipse/vendor/spdlog/include",
		"Eclipse/src",
		"Eclipse/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.imguizmo}",
		"%{IncludeDir.au}"
	}

	links {
		"Eclipse"
	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"EC_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "ECLIPSE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ECLIPSE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ECLIPSE_DIST"
		runtime "Release"
		optimize "on"
