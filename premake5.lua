workspace "Eclipse"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Eclipse"
	location "Eclipse"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs {
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines {
			"EC_PLATFORM_WINDOWS",
			"EC_BUILD_DLL"
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox" )
		}

	filter "configurations:Debug"
		defines "EC_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "EC_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "EC_DIST"
		optimize "On"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

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
		staticruntime "On"
		systemversion "latest"

		defines {
			"EC_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "EC_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "EC_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "EC_DIST"
		optimize "On"
