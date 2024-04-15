#include "etpch.h"
#include "Citro3DInput.h"


namespace Entry {

	Input* Input::s_Instance = new Citro3DInput();

	bool Citro3DInput::IsKeyPressedImpl(int keycode)
	{
		auto state = (hidKeysDownRepeat() & BIT(keycode)) >> (keycode - 1) == 1;
		return state;
	}
	bool Citro3DInput::IsScreenTouchedImpl()
	{
		auto state = (hidKeysDownRepeat() & BIT(KEY_TOUCH)) >> (KEY_TOUCH - 1) == 1;
		return state;
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
