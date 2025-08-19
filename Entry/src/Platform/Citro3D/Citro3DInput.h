#pragma once

#include "Entry/Core/Input.h"
#include "glm/glm.hpp"

namespace Entry {
	class Citro3DInput : public Input {
	protected:
		virtual bool GetButtonDownImpl(KeyCode keycode) override;
		virtual bool GetButtonImpl(KeyCode keycode) override;
		virtual bool GetButtonUpImpl(KeyCode keycode) override;

		virtual bool IsScreenTouchedImpl() override;
		virtual int GetTouchXImpl() override;
		virtual int GetTouchYImpl() override;
		virtual glm::uvec2 GetTouchPosImpl() override;
		virtual glm::vec2 GetJoystickPosImpl() override;
		virtual float GetSlider3DImpl() override;
	private:
		touchPosition m_TouchPosition;
		circlePosition m_CirclePosition;
		glm::vec2 m_JoystickPos;
		float m_Slider3DState;

	};
}