#include "etpch.h"
#include "OpenGLInput.h"

#define STICK_MAX_VALUE 154.0f

namespace Entry
{

	Input* Input::s_Instance = new OpenGLInput();

	bool OpenGLInput::GetButtonImpl(uint32_t keycode)
	{
		return false;
	}

	bool OpenGLInput::GetButtonUpImpl(uint32_t keycode)
	{
		return false;
	}

	bool OpenGLInput::GetButtonDownImpl(uint32_t keycode)
	{
		return false;
	}
	bool OpenGLInput::IsScreenTouchedImpl()
	{
		return false;
	}
	int OpenGLInput::GetTouchXImpl()
	{
		return 0;
	}
	int OpenGLInput::GetTouchYImpl()
	{
		return 0;
	}
	glm::uvec2 OpenGLInput::GetTouchPosImpl()
	{
		return glm::uvec2(0.0f);
	}

	glm::vec2 OpenGLInput::GetJoystickPosImpl()
	{
		return m_JoystickPos;
	}

	float OpenGLInput::GetSlider3DImpl()
	{
		return m_Slider3DState;
	}
}
