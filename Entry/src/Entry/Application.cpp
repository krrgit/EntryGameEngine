#include "etpch.h"
#include "Application.h"

#include "Input.h"

namespace Entry
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
#define CONSOLE_SCREEN SCREEN_NULL

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        int logScreen = SCREEN_TOP;
        ET_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;
        ET_CORE_ASSERT(logScreen > 1 && logScreen < -1, "Improper screen selected for console.");

        if (logScreen == SCREEN_NULL) 
        {
            WindowProps topProps("Top", 400, 240, SCREEN_TOP);
            std::unique_ptr<Window>(Window::Create(topProps));

            WindowProps bottomProps("Bottom", 320, 240, SCREEN_BOTTOM);
            m_Window = std::unique_ptr<Window>(Window::Create(bottomProps));
            m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        } else {
            Entry::Log::Init(logScreen);
            
            ET_CORE_WARN("Initialized Log!");
            ET_INFO("Hello!");

            uint8_t otherScreen = logScreen ^ 1;
            int width = otherScreen == SCREEN_TOP ? 400 : 320;
            std::string name = otherScreen == SCREEN_TOP ? "Top" : "Bottom";

            WindowProps props(name, width, 240, otherScreen);
            m_Window = std::unique_ptr<Window>(Window::Create(props));
            m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
        }
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