#pragma once
#include "etpch.h"
#include "Event.h"

namespace Entry {

	class ButtonEvent : public Event
	{
	public:
		inline uint32_t GetKeyCode() const { return m_ButtonCode; }

		EVENT_CLASS_CATEGORY(EventCategoryButton | EventCategoryInput)
	protected:
		ButtonEvent(uint32_t Buttoncode)
			: m_ButtonCode(Buttoncode) {}

		uint32_t m_ButtonCode;
	};

	class ButtonPressedEvent : public ButtonEvent
	{
	public:
		ButtonPressedEvent(uint32_t Buttoncode, int repeatCount)
			: ButtonEvent(Buttoncode), m_RepeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::string result = "ButtonPressedEvent: ";
			result += buttonNames[m_ButtonCode];
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
		ButtonReleasedEvent(uint32_t Buttoncode)
			: ButtonEvent(Buttoncode) {}

		std::string ToString() const override
		{
			std::string result = "ButtonReleasedEvent: ";
			result += buttonNames[m_ButtonCode];
			return result;
		}

		EVENT_CLASS_TYPE(ButtonReleased)
	};
}