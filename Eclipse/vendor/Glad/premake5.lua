project "Glad"
	kind "StaticLib"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"include/glad/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.c",
	}

	includedirs {
		"include"
	}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		
	filter "system:macosx"
		pic "On"

	filter "system:windows"
		systemversion "latest"
		staticruntime "on"

	filter { "system:windows", "configurations:Release" }
		buildoptions "/MT"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"