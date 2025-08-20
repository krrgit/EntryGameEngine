#include "etpch.h"
#include "Entry/Core/KeyCodes.h"

#include "Entry/Core/Input.h"
#include "glm/glm.hpp"

#define STICK_MAX_VALUE 154.0f

namespace Entry {

	bool Input::GetButton(KeyCode keycode)
	{
		return ((hidKeysDown() | hidKeysHeld()) & (uint32_t)keycode) == (uint32_t)keycode;
	}

	bool Input::GetButtonUp(KeyCode keycode)
	{
		return (hidKeysUp() & (uint32_t)keycode) == (uint32_t)keycode;
	}

	bool Input::GetButtonDown(KeyCode keycode)
	{
		return (hidKeysDown() & (uint32_t)keycode) == (uint32_t)keycode;
	}
	bool Input::IsScreenTouched()
	{
		return GetButton(KeyCode::PAD_TOUCH);
	}
	glm::uvec2 Input::GetTouchPos()
	{
		touchPosition touchPos;
		hidTouchRead(&touchPos);
		return glm::uvec2(touchPos.px, touchPos.py);
	}

	int Input::GetTouchX()
	{
		return GetTouchPos().x;
	}
	int Input::GetTouchY()
	{
		return GetTouchPos().y;
	}

	glm::vec2 Input::GetJoystickPos() {
		circlePosition circlePadPosition;
		hidCircleRead(&circlePadPosition);
		float x = circlePadPosition.dx / STICK_MAX_VALUE;
		float y = circlePadPosition.dy / STICK_MAX_VALUE;
		return {x, y};
	}

	float Input::GetSlider3D()
	{
		float m_Slider3DState = osGet3DSliderState();

		return m_Slider3DState;
	}
}
