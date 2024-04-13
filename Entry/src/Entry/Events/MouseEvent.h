#pragma once

#include "Event.h"

namespace Entry {

	class ScreenTouchedEvent : public Event
	{
	public:
		ScreenTouchedEvent(float x, float y)
			: m_TouchX(x), m_TouchY(y) {}

		inline float GetX() const { return m_TouchX; }
		inline float GetY() const { return m_TouchY; }

		std::string ToString() const override
		{
			return "ScreenTouchedEvent: " + std::to_string(m_TouchX) + ", " + std::to_string(m_TouchY);
		}

		EVENT_CLASS_TYPE(MouseMoved)
			EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_TouchX, m_TouchY;
	};

}