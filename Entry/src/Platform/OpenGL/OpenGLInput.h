#pragma once

#include "Entry/Core/Input.h"
#include "glm/glm.hpp"

namespace Entry
{
	class OpenGLInput : public Input {
	protected:
		virtual bool GetButtonDownImpl(uint32_t keycode) override;
		virtual bool GetButtonImpl(uint32_t keycode) override;
		virtual bool GetButtonUpImpl(uint32_t keycode) override;

		virtual bool IsScreenTouchedImpl() override;
		virtual int GetTouchXImpl() override;
		virtual int GetTouchYImpl() override;
		virtual glm::uvec2 GetTouchPosImpl() override;
		virtual glm::vec2 GetJoystickPosImpl() override;
		virtual float GetSlider3DImpl() override;
	private:
		glm::vec2 m_JoystickPos;
		float m_Slider3DState;

	};
}