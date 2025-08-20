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
			static bool GetButtonDown(KeyCode keycode);
			static bool GetButton(KeyCode keycode);
			static bool GetButtonUp(KeyCode keycode);
			static bool IsScreenTouched();
			static int GetTouchX();
			static int GetTouchY();
			static glm::uvec2 GetTouchPos();
			static glm::vec2 GetJoystickPos();
			static float GetSlider3D();

#ifdef ET_PLATFORM_WINDOWS
			static bool IsKeyPressed(KeyCode key);
			static bool IsMouseButtonPressed(const MouseCode button);
			static std::pair<float,float> GetMousePosition();
			static float GetMouseX();
			static float GetMouseY();
#endif // ET_PLATFORM_WINDOWS
	};



}