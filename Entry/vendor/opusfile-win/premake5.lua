project "opusfile"
	kind "StaticLib"
	language "C++"
    staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"include/**.h",
		"src/**.c",
		"src/**.h"
	}

	includedirs
	{
		"include",
		"../opus-win/include",
		"../libogg-win/include"
	}

	libdirs 
	{
		"../opus-win/build/Release" -- Opus.lib location (pre-built)
	}


	links
	{
		"opus"
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
