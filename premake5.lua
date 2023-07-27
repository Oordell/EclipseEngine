workspace "Eclipse"
	architecture "x86_64"
	startproject "Sandbox"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	flags {
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Eclipse/vendor/GLFW_fork/include"
IncludeDir["Glad"] = "Eclipse/vendor/GLAD/include"
IncludeDir["ImGui"] = "Eclipse/vendor/imgui"
IncludeDir["glm"] = "Eclipse/vendor/glm"
IncludeDir["stb_image"] = "Eclipse/vendor/stb_image"

group "Dependencies"
	include "Eclipse/vendor/GLFW_fork"
	include "Eclipse/vendor/Glad"
	include "Eclipse/vendor/imgui"
group ""

project "Eclipse"
	location "Eclipse"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

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
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links {
		"GLFW",
		"Glad",
		"ImGui",
		"user32.lib",
		"gdi32.lib",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"EC_PLATFORM_WINDOWS"
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


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "on"
	cppdialect "C++20"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs {
		"Eclipse/vendor/spdlog/include",
		"Eclipse/src",
		"Eclipse/vendor",
		"%{IncludeDir.glm}"
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
