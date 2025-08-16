#pragma once

#include "Entry/Core/Core.h"
#include "Entry/Core/KeyCodes.h"

#ifdef ET_PLATFORM_WINDOWS
#include "Entry/Core/MouseCodes.h"
#endif // ET_PLATFORM_WINDOWS

#include <glm/glm.hpp>

namespace Entry {

	class Input {

	public:
		inline static bool GetButtonDown(KeyCode keycode) { return s_Instance->GetButtonDownImpl(keycode); }
		inline static bool GetButton(KeyCode keycode) { return s_Instance->GetButtonImpl(keycode); }
		inline static bool GetButtonUp(KeyCode keycode) { return s_Instance->GetButtonUpImpl(keycode); }
		inline static bool IsScreenTouched() { return s_Instance->IsScreenTouchedImpl(); }
		inline static int GetTouchX() { return s_Instance->GetTouchXImpl(); }
		inline static int GetTouchY() { return s_Instance->GetTouchYImpl(); }
		inline static glm::uvec2 GetTouchPos() { return s_Instance->GetTouchPosImpl(); }
		inline static glm::vec2 GetJoystickPos() { return s_Instance->GetJoystickPosImpl(); }
		inline static float GetSlider3D() { return s_Instance->GetSlider3DImpl(); }

	protected:
		virtual bool GetButtonDownImpl(KeyCode keycode) = 0;
		virtual bool GetButtonImpl(KeyCode keycode) = 0;
		virtual bool GetButtonUpImpl(KeyCode keycode) = 0;
		virtual bool IsScreenTouchedImpl() = 0;
		virtual int GetTouchXImpl() = 0;
		virtual int GetTouchYImpl() = 0;
		virtual glm::uvec2 GetTouchPosImpl() = 0;
		virtual glm::vec2 GetJoystickPosImpl() = 0;
		virtual float GetSlider3DImpl() = 0;
	private:
		static Input* s_Instance;

	};



}