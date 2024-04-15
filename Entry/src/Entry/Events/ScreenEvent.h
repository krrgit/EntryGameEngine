#pragma once

#include "Event.h"

namespace Entry {

	class ScreenTouchedEvent : public Event
	{
	public:
		ScreenTouchedEvent(uint16_t x, uint16_t y)
			: m_TouchX(x), m_TouchY(y) {}

		inline uint16_t GetX() const { return m_TouchX; }
		inline uint16_t GetY() const { return m_TouchY; }

		std::string ToString() const override
		{
			return "ScreenTouchedEvent: " + std::to_string(m_TouchX) + ", " + std::to_string(m_TouchY);
		}

		EVENT_CLASS_TYPE(ScreenTouched)
			EVENT_CLASS_CATEGORY(EventCategoryScreen | EventCategoryInput)
	private:
		uint16_t m_TouchX, m_TouchY;
	};

	class ScreenReleasedEvent : public Event
	{
	public:
		ScreenReleasedEvent(uint16_t x, uint16_t y)
			: m_TouchX(x), m_TouchY(y) {}

		inline uint16_t GetX() const { return m_TouchX; }
		inline uint16_t GetY() const { return m_TouchY; }

		std::string ToString() const override
		{
			return "ScreenReleasedEvent: " + std::to_string(m_TouchX) + ", " + std::to_string(m_TouchY);
		}

		EVENT_CLASS_TYPE(ScreenTouched)
			EVENT_CLASS_CATEGORY(EventCategoryScreen | EventCategoryInput)
	private:
		uint16_t m_TouchX, m_TouchY;
	};
}