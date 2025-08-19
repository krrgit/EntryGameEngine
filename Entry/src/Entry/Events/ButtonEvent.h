#pragma once
#include "etpch.h"
#include "Event.h"

namespace Entry {

	class ButtonEvent : public Event
	{
	public:
		inline KeyCode GetKeyCode() const { return m_ButtonCode; }

		EVENT_CLASS_CATEGORY(EventCategoryButton | EventCategoryInput)
	protected:
		ButtonEvent(KeyCode Buttoncode)
			: m_ButtonCode(Buttoncode) {}

		KeyCode m_ButtonCode;
	};

	class ButtonPressedEvent : public ButtonEvent
	{
	public:
		ButtonPressedEvent(KeyCode Buttoncode, int repeatCount)
			: ButtonEvent(Buttoncode), m_RepeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::string result = "ButtonPressedEvent: ";
			result += buttonNames[(uint32_t)m_ButtonCode];
			result += " (" + std::to_string(m_RepeatCount) + " repeats)";
			return result;
		}

		EVENT_CLASS_TYPE(ButtonPressed)
	private:
		int m_RepeatCount;
	};

	class ButtonReleasedEvent : public ButtonEvent
	{
	public:
		ButtonReleasedEvent(KeyCode Buttoncode)
			: ButtonEvent(Buttoncode) {}

		std::string ToString() const override
		{
			std::string result = "ButtonReleasedEvent: ";
			result += buttonNames[(uint32_t)m_ButtonCode];
			return result;
		}

		EVENT_CLASS_TYPE(ButtonReleased)
	};
}