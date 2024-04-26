#include "etpch.h"
#include "Citro3DInput.h"

#define STICK_MAX_VALUE 154.0f

namespace Entry {

	Input* Input::s_Instance = new Citro3DInput();

	bool Citro3DInput::IsKeyPressedImpl(uint32_t keycode)
	{
		return ((hidKeysDown() | hidKeysHeld()) & keycode) == keycode;
	}

	bool Citro3DInput::GetButtonDownImpl(uint32_t keycode)
	{
		return (hidKeysDown() & keycode) == keycode;
	}
	bool Citro3DInput::IsScreenTouchedImpl()
	{
		return IsKeyPressedImpl(KEY_TOUCH);
	}
	int Citro3DInput::GetTouchXImpl()
	{
		hidTouchRead(&m_TouchPosition);
		return m_TouchPosition.px;
	}
	int Citro3DInput::GetTouchYImpl()
	{
		hidTouchRead(&m_TouchPosition);
		return m_TouchPosition.py;
	}
	touchPosition Citro3DInput::GetTouchPosImpl()
	{
		hidTouchRead(&m_TouchPosition);
		return m_TouchPosition;
	}

	glm::vec2 Citro3DInput::GetJoystickPosImpl() {
		circlePosition circlePos;
		hidCircleRead(&circlePos);
		m_JoystickPos.x = circlePos.dx / STICK_MAX_VALUE;
		m_JoystickPos.y = circlePos.dy / STICK_MAX_VALUE;
		return m_JoystickPos;
	}
}
