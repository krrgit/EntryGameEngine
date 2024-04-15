#pragma once

#include "Entry/Input.h"

namespace Entry {
	class Citro3DInput : public Input {
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;

		virtual bool IsScreenTouchedImpl() override;
		virtual int GetTouchXImpl() override;
		virtual int GetTouchYImpl() override;
		virtual touchPosition GetTouchPosImpl() override;
	};
}