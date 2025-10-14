workspace "EntryGameEngine"
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
IncludeDir["GLFW"] = "Entry/vendor/GLFW/include"
IncludeDir["Glad"] = "Entry/vendor/Glad/include"
IncludeDir["ImGui"] = "Entry/vendor/imgui-3ds/imgui"
IncludeDir["fast_obj"] = "Entry/vendor/fast_obj/include"
IncludeDir["glm"] = "Entry/vendor/glm"
IncludeDir["stb_image"] = "Entry/vendor/stb_image"
IncludeDir["spdlog"] = "Entry/vendor/spdlog/include"
IncludeDir["yaml_cpp"] = "Entry/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "Entry/vendor/ImGuizmo"
IncludeDir["assimp"] = "Entry/vendor/assimp/assimp/include"
IncludeDir["qu3e"] = "Entry/vendor/qu3e/src"
IncludeDir["angelscript"] = "Entry/vendor/angelscript/sdk/angelscript/include"
IncludeDir["opus_win"] = "Entry/vendor/opus-win/include"
IncludeDir["libogg_win"] = "Entry/vendor/libogg-win/include"
IncludeDir["opusfile_win"] = "Entry/vendor/opusfile-win/include"
IncludeDir["miniaudio_win"] = "Entry/vendor/miniaudio-win"

group "Dependencies"
	include "Entry/vendor/GLFW"
	include "Entry/vendor/Glad"
	include "Entry/vendor/imgui-3ds"
	include "Entry/vendor/yaml-cpp"
	include "Entry/vendor/assimp"
	include "Entry/vendor/qu3e"
	include "Entry/vendor/angelscript/sdk/angelscript"
	include "Entry/vendor/libogg-win"
	include "Entry/vendor/opusfile-win"
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
		"%{prj.name}/vendor/spdlog/include/**.h",
		"%{prj.name}/vendor/imgui-3ds/imgui/backends/**.h",
		"%{prj.name}/vendor/imgui-3ds/imgui/backends/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp",
		"%{prj.name}/vendor/miniaudio-win/miniaudio.h",
		"%{prj.name}/vendor/miniaudio-win/miniaudio.cpp",
		"%{prj.name}/vendor/miniaudio-win/extras/**.h",
		"%{prj.name}/vendor/miniaudio-win/extras/**.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE"
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
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.qu3e}",
		"%{IncludeDir.opus_win}",
		"%{IncludeDir.libogg_win}",
		"%{IncludeDir.opusfile_win}",
		"%{IncludeDir.miniaudio_win}"
	}

	libdirs 
	{
		"Entry/vendor/opus-win/build/Release" -- Opus.lib location (pre-built)
	}

	links 
	{
		"GLFW",
		"Glad",
		"ImGui",
		"yaml-cpp",
		"opengl32.lib",
		"dwmapi.lib",
		"assimp",
		"qu3e",
		"opusfile",
		"opus",
		"libogg"
	}

	filter "files:Entry/vendor/ImGuizmo/**.cpp"
    flags { "NoPCH" }
	
	filter "files:Entry/vendor/tinygltf/**.cpp"
    flags { "NoPCH" }

	filter "files:Entry/vendor/miniaudio-win/**.cpp"
    flags { "NoPCH" }

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

project "Sandbox" -- APPLICATION
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++11"
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
		"%{IncludeDir.yaml_cpp}",
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

project "Entryway" -- EDITOR
	location "Entryway"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"  -- Maybe keep on C++11 ?
	staticruntime "on"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"Entry/vendor/spdlog/include",
		"Entry/src",
		"Entry/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.assimp}",
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