#include "etpch.h"
#include "WindowsInput.h"

#include "Entry/Core/Application.h"
#include "Entry/Core//KeyCodes.h"

#include <GLFW/glfw3.h>

#define STICK_MAX_VALUE 154.0f

namespace Entry
{

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::GetButtonImpl(KeyCode keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::GetButtonUpImpl(KeyCode keycode)
	{
		return false;
	}

	bool WindowsInput::GetButtonDownImpl(KeyCode keycode)
	{
		return false;
	}
	bool WindowsInput::IsScreenTouchedImpl()
	{
		return false;
	}
	int WindowsInput::GetTouchXImpl()
	{
		return 0;
	}
	int WindowsInput::GetTouchYImpl()
	{
		return 0;
	}
	glm::uvec2 WindowsInput::GetTouchPosImpl()
	{
		return glm::uvec2(0.0f);
	}

	glm::vec2 WindowsInput::GetJoystickPosImpl()
	{
		m_JoystickPos.x = (GetButtonImpl(ET_PAD_CPAD_RIGHT) ? 1 : 0) - (GetButtonImpl(ET_PAD_CPAD_LEFT) ? 1 : 0);
		m_JoystickPos.y = (GetButtonImpl(ET_PAD_CPAD_UP) ? 1 : 0) - (GetButtonImpl(ET_PAD_CPAD_DOWN) ? 1 : 0);
		
		return m_JoystickPos;
	}

	float WindowsInput::GetSlider3DImpl()
	{
		return m_Slider3DState;
	}
}
