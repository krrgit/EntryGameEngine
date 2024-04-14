#include "etpch.h"
#include "Application.h"

namespace Entry
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        ET_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        WindowProps topProps("Bottom", 320, 240, SCREEN_BOTTOM);
        m_Window = std::unique_ptr<Window>(Window::Create(topProps));
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
    }

    Application::~Application()
    {
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer)
    {
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
                break;
        }
        
    }

    void Application::Run()
    {

        while (aptMainLoop() && m_Running) {
            m_Window->FrameBegin();

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();


            m_Window->OnUpdate();

            m_Window->FrameEnd();
        }
    }
}