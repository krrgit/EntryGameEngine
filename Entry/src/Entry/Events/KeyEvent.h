#pragma once

#include "Event.h"

namespace Entry {

	class KeyEvent : public Event
	{
	public:
		inline uint32_t GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(uint32_t keycode)
			: m_KeyCode(keycode) {}

		uint32_t m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(uint32_t keycode, int repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::string result = "KeyPressedEvent: ";
			result += keysNames[m_KeyCode];
			result += " (" + std::to_string(m_RepeatCount) + " repeats)";
			return result;
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(uint32_t keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::string result = "KeyReleasedEvent: ";
			result += keysNames[m_KeyCode];
			return result;
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}