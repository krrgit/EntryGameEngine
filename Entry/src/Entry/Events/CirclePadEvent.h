#pragma once

#include "Event.h"

namespace Entry {

	class CirclePadEvent : public Event
	{
	public:
		CirclePadEvent(float x, float y)
			: m_PadX(x), m_PadY(y) {}

		inline float GetX() const { return m_PadX; }
		inline float GetY() const { return m_PadY; }

		std::string ToString() const override
		{
			return "CirclePadEvent: " + std::to_string(m_PadX) + ", " + std::to_string(m_PadY);
		}

		EVENT_CLASS_TYPE(CirclePadMoved)
			EVENT_CLASS_CATEGORY(EventCategoryCirclePad | EventCategoryInput)
	private:
		uint16_t m_PadX, m_PadY;
	};
}