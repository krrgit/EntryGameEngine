#include "etpch.h"
#include "Citro3DInput.h"
#include "Entry/Core/KeyCodes.h"

#define STICK_MAX_VALUE 154.0f

namespace Entry {

	Input* Input::s_Instance = new Citro3DInput();

	bool Citro3DInput::GetButtonImpl(KeyCode keycode)
	{
		return ((hidKeysDown() | hidKeysHeld()) & (uint32_t)keycode) == (uint32_t)keycode;
	}

	bool Citro3DInput::GetButtonUpImpl(KeyCode keycode)
	{
		return (hidKeysUp() & (uint32_t)keycode) == (uint32_t)keycode;
	}

	bool Citro3DInput::GetButtonDownImpl(KeyCode keycode)
	{
		return (hidKeysDown() & (uint32_t)keycode) == (uint32_t)keycode;
	}
	bool Citro3DInput::IsScreenTouchedImpl()
	{
		return GetButtonImpl(KeyCode::PAD_TOUCH);
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
	glm::uvec2 Citro3DInput::GetTouchPosImpl()
	{
		hidTouchRead(&m_TouchPosition);
		return glm::uvec2 (m_TouchPosition.px, m_TouchPosition.py);
	}

	glm::vec2 Citro3DInput::GetJoystickPosImpl() {
		hidCircleRead(&m_CirclePosition);
		m_JoystickPos.x = m_CirclePosition.dx / STICK_MAX_VALUE;
		m_JoystickPos.y = m_CirclePosition.dy / STICK_MAX_VALUE;
		return m_JoystickPos;
	}

	float Citro3DInput::GetSlider3DImpl()
	{
		m_Slider3DState = osGet3DSliderState();

		return m_Slider3DState;
	}
}
