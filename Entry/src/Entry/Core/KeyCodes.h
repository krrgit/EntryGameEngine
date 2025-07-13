#pragma once

#ifndef BIT
 #define BIT(n) (1U<<(n))
#endif

// from Citro3D
#define ET_KEY_A			BIT(0)      ///< A
#define ET_KEY_B			BIT(1)      ///< B
#define ET_KEY_SELECT		BIT(2)      ///< Select
#define ET_KEY_START		BIT(3)      ///< Start
#define ET_KEY_DRIGHT		BIT(4)      ///< D-Pad Right
#define ET_KEY_DLEFT		BIT(5)      ///< D-Pad Left
#define ET_KEY_DUP			BIT(6)      ///< D-Pad Up
#define ET_KEY_DDOWN		BIT(7)      ///< D-Pad Down
#define ET_KEY_R			BIT(8)      ///< R
#define ET_KEY_L			BIT(9)      ///< L
#define ET_KEY_X			BIT(10)     ///< X
#define ET_KEY_Y			BIT(11)     ///< Y
#define ET_KEY_ZL			BIT(14)     ///< ZL (New 3DS only)
#define ET_KEY_ZR			BIT(15)     ///< ZR (New 3DS only)
#define ET_KEY_TOUCH		BIT(20)     ///< Touch (Not actually provided by HID)
#define ET_KEY_CSTICK_RIGHT BIT(24)		///< C-Stick Right (New 3DS only)
#define ET_KEY_CSTICK_LEFT  BIT(25)		///< C-Stick Left (New 3DS only)
#define ET_KEY_CSTICK_UP	BIT(26)		///< C-Stick Up (New 3DS only)
#define ET_KEY_CSTICK_DOWN  BIT(27)		///< C-Stick Down (New 3DS only)
#define ET_KEY_CPAD_RIGHT	BIT(28)		///< Circle Pad Right
#define ET_KEY_CPAD_LEFT	BIT(29)		///< Circle Pad Left
#define ET_KEY_CPAD_UP		BIT(30)		///< Circle Pad Up
#define ET_KEY_CPAD_DOWN	BIT(31)		///< Circle Pad Down

// Generic catch-all directions
#define ET_KEY_UP     ET_KEY_DUP    | ET_KEY_CPAD_UP    ///< D-Pad Up or Circle Pad Up
#define ET_KEY_DOWN   ET_KEY_DDOWN  | ET_KEY_CPAD_DOWN  ///< D-Pad Down or Circle Pad Down
#define ET_KEY_LEFT   ET_KEY_DLEFT  | ET_KEY_CPAD_LEFT  ///< D-Pad Left or Circle Pad Left
#define ET_KEY_RIGHT  ET_KEY_DRIGHT | ET_KEY_CPAD_RIGHT ///< D-Pad Right or Circle Pad Right