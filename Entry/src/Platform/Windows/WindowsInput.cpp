#include "etpch.h"

#include "Entry/Core/Application.h"
#include "Entry/Core//KeyCodes.h"
#include "Entry/Core/Input.h"

#include <GLFW/glfw3.h>

#define STICK_MAX_VALUE 154.0f

namespace Entry
{
	bool Input::GetButton(KeyCode keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<uint32_t>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::GetButtonUp(KeyCode keycode)
	{
		return false;
	}

	bool Input::GetButtonDown(KeyCode keycode)
	{
		return false;
	}
	bool Input::IsScreenTouched()
	{
		return false;
	}
	int Input::GetTouchX()
	{
		return 0;
	}
	int Input::GetTouchY()
	{
		return 0;
	}
	glm::uvec2 Input::GetTouchPos()
	{
		return glm::uvec2(0.0f);
	}

	glm::vec2 Input::GetJoystickPos()
	{
		int x = (GetButton(ET_PAD_CPAD_RIGHT) ? 1 : 0) - (GetButton(ET_PAD_CPAD_LEFT) ? 1 : 0);
		int y = (GetButton(ET_PAD_CPAD_UP) ? 1 : 0) - (GetButton(ET_PAD_CPAD_DOWN) ? 1 : 0);
		
		return {x, y};
	}

	float Input::GetSlider3D()
	{
		return 0.0f;
	}

	bool Input::IsKeyPressed(KeyCode key) 
	{
		return GetButton(key);
	}

	bool Input::IsMouseButtonPressed(const MouseCode button)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonReleased(const MouseCode button)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_RELEASE;
	}

	std::pair<float, float> Input::GetMousePosition() 
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return  { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX() 
	{
		std::pair<float,float> mousePos = GetMousePosition();
		return mousePos.first;
	}

	float Input::GetMouseY() 
	{
		std::pair<float, float> mousePos = GetMousePosition();
		return mousePos.second;
	}
}
