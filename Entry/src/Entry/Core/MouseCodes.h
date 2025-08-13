#pragma once

namespace Entry
{
	typedef enum class MouseCode : uint16_t
	{
		// From glfw3.h
		Button0                = 0,
		Button1                = 1,
		Button2                = 2,
		Button3                = 3,
		Button4                = 4,
		Button5                = 5,
		Button6                = 6,
		Button7                = 7,

		ButtonLast             = Button7,
		ButtonLeft             = Button0,
		ButtonRight            = Button1,
		ButtonMiddle           = Button2
	} Mouse;
	
	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}

#define ET_MOUSE_BUTTON_0      ::Entry::Mouse::Button0
#define ET_MOUSE_BUTTON_1      ::Entry::Mouse::Button1
#define ET_MOUSE_BUTTON_2      ::Entry::Mouse::Button2
#define ET_MOUSE_BUTTON_3      ::Entry::Mouse::Button3
#define ET_MOUSE_BUTTON_4      ::Entry::Mouse::Button4
#define ET_MOUSE_BUTTON_5      ::Entry::Mouse::Button5
#define ET_MOUSE_BUTTON_6      ::Entry::Mouse::Button6
#define ET_MOUSE_BUTTON_7      ::Entry::Mouse::Button7
#define ET_MOUSE_BUTTON_LAST   ::Entry::Mouse::ButtonLast
#define ET_MOUSE_BUTTON_LEFT   ::Entry::Mouse::ButtonLeft
#define ET_MOUSE_BUTTON_RIGHT  ::Entry::Mouse::ButtonRight
#define ET_MOUSE_BUTTON_MIDDLE ::Entry::Mouse::ButtonMiddle
