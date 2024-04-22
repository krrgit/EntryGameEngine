#pragma once

#include "Entry/Core.h"
#include "glm/glm.hpp"
#include "KeyCodes.h"

namespace Entry {

	class Input {

	public:
		inline static bool IsKeyPressed(uint32_t keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		inline static bool IsScreenTouched() { return s_Instance->IsScreenTouchedImpl(); }
		inline static int GetTouchX() { return s_Instance->GetTouchXImpl(); }
		inline static int GetTouchY() { return s_Instance->GetTouchYImpl(); }
		inline static touchPosition GetTouchPos() { return s_Instance->GetTouchPosImpl(); }
		inline static glm::vec2 GetJoystickPos() { return s_Instance->GetJoystickPosImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(uint32_t keycode) = 0;

		virtual bool IsScreenTouchedImpl() = 0;
		virtual int GetTouchXImpl() = 0;
		virtual int GetTouchYImpl() = 0;
		virtual touchPosition GetTouchPosImpl() = 0;
		virtual glm::vec2 GetJoystickPosImpl() = 0;
	private:
		static Input* s_Instance;

	};



}