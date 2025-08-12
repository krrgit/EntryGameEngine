#include "etpch.h"
#include "Entry/Utils/PlatformUtils.h"
#include "Entry/Core/Application.h"

#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Entry {

	float Time::GetTime()
	{
		return glfwGetTime();
	}
}
