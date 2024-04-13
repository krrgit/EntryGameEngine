#include "Application.h"

#include "Log.h"


namespace Entry
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application::Application()
    {
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
    }

    Application::~Application()
    {
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);

        ET_CORE_TRACE("{0}", e);
    }

    void Application::Run()
    {
       
        while (aptMainLoop() && m_Running) {
            m_Window->OnUpdate();
        }
    }
}