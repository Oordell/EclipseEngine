workspace "Eclipse"
	architecture "x64"
	startproject "Sandbox"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Eclipse/vendor/GLFW_fork/include"
IncludeDir["Glad"] = "Eclipse/vendor/GLAD/include"
IncludeDir["ImGui"] = "Eclipse/vendor/imgui"

group "Dependencies"
	include "Eclipse/vendor/GLFW_fork"
	include "Eclipse/vendor/Glad"
	include "Eclipse/vendor/imgui"
group ""

project "Eclipse"
	location "Eclipse"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "ecpch.h"
	pchsource "%{prj.name}/src/ecpch.cpp"

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
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
		cppdialect "C++20"
		systemversion "latest"

		defines {
			"EC_PLATFORM_WINDOWS",
			"EC_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"" )
		}

	filter "configurations:Debug"
		defines "EC_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "EC_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "EC_DIST"
		runtime "Release"
		optimize "On"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs {
		"Eclipse/vendor/spdlog/include",
		"Eclipse/src"
	}

	links {
		"Eclipse"
	}

	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"

		defines {
			"EC_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "EC_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "EC_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "EC_DIST"
		runtime "Release"
		optimize "On"
