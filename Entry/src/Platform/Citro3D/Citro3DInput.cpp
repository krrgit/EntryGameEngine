#include "etpch.h"
#include "Citro3DInput.h"


namespace Entry {

	Input* Input::s_Instance = new Citro3DInput();

	bool Citro3DInput::IsKeyPressedImpl(uint32_t keycode)
	{
		return ((hidKeysDownRepeat() & keycode) == keycode);
	}
	bool Citro3DInput::IsScreenTouchedImpl()
	{
		return IsKeyPressedImpl(KEY_TOUCH);
	}
	int Citro3DInput::GetTouchXImpl()
	{
		touchPosition pos;
		hidTouchRead(&pos);
		return pos.px;
	}
	int Citro3DInput::GetTouchYImpl()
	{
		touchPosition pos;
		hidTouchRead(&pos);
		return pos.py;
	}
	touchPosition Citro3DInput::GetTouchPosImpl()
	{
		touchPosition pos;
		hidTouchRead(&pos);
		return pos;
	}
}
