#pragma once

#include "Entry/Core/Input.h"
#include "glm/glm.hpp"

namespace Entry {
	class Citro3DInput : public Input {
	protected:
		virtual bool GetButtonDownImpl(uint32_t keycode) override;
		virtual bool GetButtonImpl(uint32_t keycode) override;
		virtual bool GetButtonUpImpl(uint32_t keycode) override;

		virtual bool IsScreenTouchedImpl() override;
		virtual int GetTouchXImpl() override;
		virtual int GetTouchYImpl() override;
		virtual touchPosition GetTouchPosImpl() override;
		virtual glm::vec2 GetJoystickPosImpl() override;
	private:
		touchPosition m_TouchPosition;
		glm::vec2 m_JoystickPos;

	};
}