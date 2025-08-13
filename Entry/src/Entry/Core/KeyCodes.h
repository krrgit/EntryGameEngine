#pragma once

#ifndef BIT
 #define BIT(n) (1U<<(n))
#endif

// from Citro3D
#define ET_BTN_A			BIT(0)      ///< A
#define ET_BTN_B			BIT(1)      ///< B
#define ET_BTN_SELECT		BIT(2)      ///< Select
#define ET_BTN_START		BIT(3)      ///< Start
#define ET_BTN_DRIGHT		BIT(4)      ///< D-Pad Right
#define ET_BTN_DLEFT		BIT(5)      ///< D-Pad Left
#define ET_BTN_DUP			BIT(6)      ///< D-Pad Up
#define ET_BTN_DDOWN		BIT(7)      ///< D-Pad Down
#define ET_BTN_R			BIT(8)      ///< R
#define ET_BTN_L			BIT(9)      ///< L
#define ET_BTN_X			BIT(10)     ///< X
#define ET_BTN_Y			BIT(11)     ///< Y
#define ET_BTN_ZL			BIT(14)     ///< ZL (New 3DS only)
#define ET_BTN_ZR			BIT(15)     ///< ZR (New 3DS only)
#define ET_BTN_TOUCH		BIT(20)     ///< Touch (Not actually provided by HID)
#define ET_BTN_CSTICK_RIGHT BIT(24)		///< C-Stick Right (New 3DS only)
#define ET_BTN_CSTICK_LEFT  BIT(25)		///< C-Stick Left (New 3DS only)
#define ET_BTN_CSTICK_UP	BIT(26)		///< C-Stick Up (New 3DS only)
#define ET_BTN_CSTICK_DOWN  BIT(27)		///< C-Stick Down (New 3DS only)
#define ET_BTN_CPAD_RIGHT	BIT(28)		///< Circle Pad Right
#define ET_BTN_CPAD_LEFT	BIT(29)		///< Circle Pad Left
#define ET_BTN_CPAD_UP		BIT(30)		///< Circle Pad Up
#define ET_BTN_CPAD_DOWN	BIT(31)		///< Circle Pad Down

// Generic catch-all directions
#define ET_BTN_UP     ET_BTN_DUP    | ET_BTN_CPAD_UP    ///< D-Pad Up or Circle Pad Up
#define ET_BTN_DOWN   ET_BTN_DDOWN  | ET_BTN_CPAD_DOWN  ///< D-Pad Down or Circle Pad Down
#define ET_BTN_LEFT   ET_BTN_DLEFT  | ET_BTN_CPAD_LEFT  ///< D-Pad Left or Circle Pad Left
#define ET_BTN_RIGHT  ET_BTN_DRIGHT | ET_BTN_CPAD_RIGHT ///< D-Pad Right or Circle Pad Right

#ifdef ET_PLATFORM_WINDOWS

namespace Entry
{
	typedef enum class Win_KeyCode : uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, Win_KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

// From glfw3.h
#define ET_KEY_SPACE           ::Entry::Key::Space
#define ET_KEY_APOSTROPHE      ::Entry::Key::Apostrophe    /* ' */
#define ET_KEY_COMMA           ::Entry::Key::Comma         /* , */
#define ET_KEY_MINUS           ::Entry::Key::Minus         /* - */
#define ET_KEY_PERIOD          ::Entry::Key::Period        /* . */
#define ET_KEY_SLASH           ::Entry::Key::Slash         /* / */
#define ET_KEY_0               ::Entry::Key::D0
#define ET_KEY_1               ::Entry::Key::D1
#define ET_KEY_2               ::Entry::Key::D2
#define ET_KEY_3               ::Entry::Key::D3
#define ET_KEY_4               ::Entry::Key::D4
#define ET_KEY_5               ::Entry::Key::D5
#define ET_KEY_6               ::Entry::Key::D6
#define ET_KEY_7               ::Entry::Key::D7
#define ET_KEY_8               ::Entry::Key::D8
#define ET_KEY_9               ::Entry::Key::D9
#define ET_KEY_SEMICOLON       ::Entry::Key::Semicolon     /* ; */
#define ET_KEY_EQUAL           ::Entry::Key::Equal         /* = */
#define ET_KEY_A               ::Entry::Key::A
#define ET_KEY_B               ::Entry::Key::B
#define ET_KEY_C               ::Entry::Key::C
#define ET_KEY_D               ::Entry::Key::D
#define ET_KEY_E               ::Entry::Key::E
#define ET_KEY_F               ::Entry::Key::F
#define ET_KEY_G               ::Entry::Key::G
#define ET_KEY_H               ::Entry::Key::H
#define ET_KEY_I               ::Entry::Key::I
#define ET_KEY_J               ::Entry::Key::J
#define ET_KEY_K               ::Entry::Key::K
#define ET_KEY_L               ::Entry::Key::L
#define ET_KEY_M               ::Entry::Key::M
#define ET_KEY_N               ::Entry::Key::N
#define ET_KEY_O               ::Entry::Key::O
#define ET_KEY_P               ::Entry::Key::P
#define ET_KEY_Q               ::Entry::Key::Q
#define ET_KEY_R               ::Entry::Key::R
#define ET_KEY_S               ::Entry::Key::S
#define ET_KEY_T               ::Entry::Key::T
#define ET_KEY_U               ::Entry::Key::U
#define ET_KEY_V               ::Entry::Key::V
#define ET_KEY_W               ::Entry::Key::W
#define ET_KEY_X               ::Entry::Key::X
#define ET_KEY_Y               ::Entry::Key::Y
#define ET_KEY_Z               ::Entry::Key::Z
#define ET_KEY_LEFT_BRACKET    ::Entry::Key::LeftBracket   /* [ */
#define ET_KEY_BACKSLASH       ::Entry::Key::Backslash     /* \ */
#define ET_KEY_RIGHT_BRACKET   ::Entry::Key::RightBracket  /* ] */
#define ET_KEY_GRAVE_ACCENT    ::Entry::Key::GraveAccent   /* ` */
#define ET_KEY_WORLD_1         ::Entry::Key::World1        /* non-US #1 */
#define ET_KEY_WORLD_2         ::Entry::Key::World2        /* non-US #2 */

/* Function keys */
#define ET_KEY_ESCAPE          ::Entry::Key::Escape
#define ET_KEY_ENTER           ::Entry::Key::Enter
#define ET_KEY_TAB             ::Entry::Key::Tab
#define ET_KEY_BACKSPACE       ::Entry::Key::Backspace
#define ET_KEY_INSERT          ::Entry::Key::Insert
#define ET_KEY_DELETE          ::Entry::Key::Delete
#define ET_KEY_RIGHT           ::Entry::Key::Right
#define ET_KEY_LEFT            ::Entry::Key::Left
#define ET_KEY_DOWN            ::Entry::Key::Down
#define ET_KEY_UP              ::Entry::Key::Up
#define ET_KEY_PAGE_UP         ::Entry::Key::PageUp
#define ET_KEY_PAGE_DOWN       ::Entry::Key::PageDown
#define ET_KEY_HOME            ::Entry::Key::Home
#define ET_KEY_END             ::Entry::Key::End
#define ET_KEY_CAPS_LOCK       ::Entry::Key::CapsLock
#define ET_KEY_SCROLL_LOCK     ::Entry::Key::ScrollLock
#define ET_KEY_NUM_LOCK        ::Entry::Key::NumLock
#define ET_KEY_PRINT_SCREEN    ::Entry::Key::PrintScreen
#define ET_KEY_PAUSE           ::Entry::Key::Pause
#define ET_KEY_F1              ::Entry::Key::F1
#define ET_KEY_F2              ::Entry::Key::F2
#define ET_KEY_F3              ::Entry::Key::F3
#define ET_KEY_F4              ::Entry::Key::F4
#define ET_KEY_F5              ::Entry::Key::F5
#define ET_KEY_F6              ::Entry::Key::F6
#define ET_KEY_F7              ::Entry::Key::F7
#define ET_KEY_F8              ::Entry::Key::F8
#define ET_KEY_F9              ::Entry::Key::F9
#define ET_KEY_F10             ::Entry::Key::F10
#define ET_KEY_F11             ::Entry::Key::F11
#define ET_KEY_F12             ::Entry::Key::F12
#define ET_KEY_F13             ::Entry::Key::F13
#define ET_KEY_F14             ::Entry::Key::F14
#define ET_KEY_F15             ::Entry::Key::F15
#define ET_KEY_F16             ::Entry::Key::F16
#define ET_KEY_F17             ::Entry::Key::F17
#define ET_KEY_F18             ::Entry::Key::F18
#define ET_KEY_F19             ::Entry::Key::F19
#define ET_KEY_F20             ::Entry::Key::F20
#define ET_KEY_F21             ::Entry::Key::F21
#define ET_KEY_F22             ::Entry::Key::F22
#define ET_KEY_F23             ::Entry::Key::F23
#define ET_KEY_F24             ::Entry::Key::F24
#define ET_KEY_F25             ::Entry::Key::F25

/* Keypad */
#define ET_KEY_KP_0            ::Entry::Key::KP0
#define ET_KEY_KP_1            ::Entry::Key::KP1
#define ET_KEY_KP_2            ::Entry::Key::KP2
#define ET_KEY_KP_3            ::Entry::Key::KP3
#define ET_KEY_KP_4            ::Entry::Key::KP4
#define ET_KEY_KP_5            ::Entry::Key::KP5
#define ET_KEY_KP_6            ::Entry::Key::KP6
#define ET_KEY_KP_7            ::Entry::Key::KP7
#define ET_KEY_KP_8            ::Entry::Key::KP8
#define ET_KEY_KP_9            ::Entry::Key::KP9
#define ET_KEY_KP_DECIMAL      ::Entry::Key::KPDecimal
#define ET_KEY_KP_DIVIDE       ::Entry::Key::KPDivide
#define ET_KEY_KP_MULTIPLY     ::Entry::Key::KPMultiply
#define ET_KEY_KP_SUBTRACT     ::Entry::Key::KPSubtract
#define ET_KEY_KP_ADD          ::Entry::Key::KPAdd
#define ET_KEY_KP_ENTER        ::Entry::Key::KPEnter
#define ET_KEY_KP_EQUAL        ::Entry::Key::KPEqual

#define ET_KEY_LEFT_SHIFT      ::Entry::Key::LeftShift
#define ET_KEY_LEFT_CONTROL    ::Entry::Key::LeftControl
#define ET_KEY_LEFT_ALT        ::Entry::Key::LeftAlt
#define ET_KEY_LEFT_SUPER      ::Entry::Key::LeftSuper
#define ET_KEY_RIGHT_SHIFT     ::Entry::Key::RightShift
#define ET_KEY_RIGHT_CONTROL   ::Entry::Key::RightControl
#define ET_KEY_RIGHT_ALT       ::Entry::Key::RightAlt
#define ET_KEY_RIGHT_SUPER     ::Entry::Key::RightSuper
#define ET_KEY_MENU            ::Entry::Key::Menu

#endif
