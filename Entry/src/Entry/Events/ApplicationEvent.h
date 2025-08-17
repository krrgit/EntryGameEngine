#pragma once

#include "Event.h"

namespace Entry {

	class  WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height, void* window)
			: m_Width(width), m_Height(height), m_Window(window) {}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }
		inline void* GetWindow() const { return m_Window; }

		std::string ToString() const override
		{
			return "WindowResizeEvent: " + std::to_string(m_Width) + ", " + std::to_string(m_Height);
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		unsigned int m_Width, m_Height;
		void* m_Window;
	};

	class  WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class  AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class  AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}