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
IncludeDir["GLFW"] = "Entry/vendor/GLFW/include"
IncludeDir["Glad"] = "Entry/vendor/Glad/include"
IncludeDir["ImGui"] = "Entry/vendor/imgui-3ds/imgui"
IncludeDir["fast_obj"] = "Entry/vendor/fast_obj"
IncludeDir["glm"] = "Entry/vendor/glm"
IncludeDir["stb_image"] = "Entry/vendor/stb_image"
IncludeDir["spdlog"] = "Entry/vendor/spdlog/include"

group "Dependencies"
	include "Entry/vendor/GLFW"
	-- include "Entry/vendor/Glad"
	include "Entry/vendor/imgui-3ds"

group ""

project "Entry"
	location "Entry"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "etpch.h"
	pchsource "Entry/src/etpch.cpp"
	
	files 
	{
		"%{prj.name}/src/*.h",
		"%{prj.name}/src/*.cpp",
		"%{prj.name}/src/Entry/**.h",
		"%{prj.name}/src/Entry/**.cpp",
		"%{prj.name}/src/Platform/OpenGL/**.h",
		"%{prj.name}/src/Platform/OpenGL/**.cpp",
		"%{prj.name}/src/Platform/Windows/**.h",
		"%{prj.name}/src/Platform/Windows/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/spdlog/include/**.h"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.fast_obj}",
	}

	links 
	{
		"GLFW",
		-- "Glad",
		"ImGui",
		"opengl32.lib",
		"dwmapi.lib"
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

	filter "files:../imgui_sw.cpp"
		flags { "ExcludeFromBuild" }

	filter "files:../imgui/imgui_sw.h"
		flags { "ExcludeFromBuild" }

	filter "files:../Log/LogLayer.cpp"
		flags { "ExcludeFromBuild" }

	filter "files:../Log/LogLayer.h"
		flags { "ExcludeFromBuild" }

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
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Entry/vendor/spdlog/include",
		"Entry/src",
		"Entry/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
	}

	links
	{
		"Entry"
	}

	filter "system:windows"
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