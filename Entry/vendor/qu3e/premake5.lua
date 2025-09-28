project "qu3e"
	kind "StaticLib"
	language "C++"
    staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++11"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++11"

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
