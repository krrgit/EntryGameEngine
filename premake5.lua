workspace "EntryGameEngine"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
-- IncludeDir["GLFW"] = "Entry/vendor/GLFW/include"
-- IncludeDir["Glad"] = "Entry/vendor/Glad/include"
IncludeDir["ImGui"] = "Entry/vendor/imgui"
IncludeDir["fast_obj"] = "Entry/vendor/fast_obj"
-- IncludeDir["glm"] = "Entry/vendor/glm"
-- IncludeDir["stb_image"] = "Entry/vendor/stb_image"

project "Entry"
	location "Entry"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "etpch.h"
	pchsource "Entry/src/etpch.cpp"
	
	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		-- "%{IncludeDir.GLFW}",
		-- "%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		-- "%{IncludeDir.glm}",
		"%{IncludeDir.fast_obj}",
	}

	filter "system:windows"
		cppdialect "C++11"
		staticruntime "On"
		systemversion "latest"

		defines 
		{
			"ET_PLATFORM_WINDOWS",
			"ET_BUILD_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter { "system:windows", "files:Citro3D**.*"}
  		flags { "ExcludeFromBuild"}

	filter "configurations:Debug"
		defines "ET_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "ET_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "ET_DIST"
		optimize "On"

	filter {"system:windows", "configurations:Release"}
		buildoptions "/MT"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Entry/vendor/spdlog/spdlog/include",
		"Entry/vendor/spdlog/imgui-3ds/imgui",	
		"Entry/vendor/spdlog/glm/glm",
		"Entry/src"
	}

	links
	{
		"Entry"
	}

	filter "system:windows"
		cppdialect "C++11"
		staticruntime "On"
		systemversion "latest"

		defines 
		{
			"ET_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "ET_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "ET_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "ET_DIST"
		optimize "On"

	filter {"system:windows", "configurations:Release"}
		buildoptions "/MT"