#pragma once

#include "Event.h"

namespace Entry
{

	class Slider3DEvent : public Event
	{
	public:
		Slider3DEvent(float sliderValue)
			: m_SliderValue(sliderValue)
		{
		}

		inline float GetValue() const { return m_SliderValue; }

		std::string ToString() const override
		{
			return "Slider3DEvent: " + std::to_string(m_SliderValue);
		}

		EVENT_CLASS_TYPE(Slider3DMoved)
			EVENT_CLASS_CATEGORY(EventCategorySlider3D | EventCategoryInput)
	private:
		float m_SliderValue;
	};
}