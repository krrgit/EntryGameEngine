#include "Application.h"

#include "Entry/Events/ApplicationEvent.h"
#include "Log.h"


namespace Entry
{
    Application::Application()
    {
        m_Window = std::unique_ptr<Window>(Window::Create());
    }

    Application::~Application()
    {
    }

    void Application::Run()
    {
       
        while (aptMainLoop() && m_Running) {
            m_Window->OnUpdate();
        }
    }
}