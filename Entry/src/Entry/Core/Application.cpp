#include "etpch.h"
#include "Application.h"

#include "Input.h"
#include "Entry/Renderer/Renderer.h"
#include "Entry/Core/Timestep.h"
#include <time.h>
#include <memory>

namespace Entry
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        Renderer::Init();
        Log::Init();

        ET_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        WindowProps topProps("Top", 400, 240, GFX_TOP);
        m_WindowTop = Scope<Window>(Window::Create(topProps));
        m_CurrentWindow = m_WindowTop.get();
        RenderCommand::SetClearColor(0x68B0D8FF);

        WindowProps props("Bottom", 320, 240, GFX_BOTTOM);
        m_WindowBottom = Scope<Window>(Window::Create(props));
        m_WindowBottom->SetEventCallback(BIND_EVENT_FN(OnEvent));
        m_CurrentWindow = m_WindowBottom.get();
        RenderCommand::SetClearColor(0x252525FF);

        m_ImGuiLayer = new Entry::ImGuiLayer();
        PushOverlay(m_ImGuiLayer, ET_WINDOW_BOTTOM);

        osTickCounterStart(&m_FrameTime);
    }


    Application::~Application()
    {
    }

    void Application::PushLayer(Layer* layer, int window)
    {
        if (window == ET_WINDOW_TOP)
            m_WindowTop->PushLayer(layer);
        else if (window == ET_WINDOW_BOTTOM)
            m_WindowBottom->PushLayer(layer);
    }

    void Application::PushOverlay(Layer* layer, int window)
    {
        if (window == ET_WINDOW_TOP)
            m_WindowTop->PushOverlay(layer);
        else if (window == ET_WINDOW_BOTTOM)
            m_WindowBottom->PushOverlay(layer);
    }

    void Application::OnEvent(Event& e)
    {
        m_WindowTop->OnEvent(e);
        m_WindowBottom->OnEvent(e);
    }

    void Application::Run()
    {
        
        while (aptMainLoop() && m_Running) {
            C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

            osTickCounterUpdate(&m_FrameTime);
            Timestep timestep = osTickCounterRead(&m_FrameTime) * 0.001f;
            osTickCounterStart(&m_FrameTime);

            m_ImGuiLayer->Begin(timestep);

            m_CurrentWindow = m_WindowTop.get();
            m_CurrentWindow->FrameDrawOn();

            m_CurrentWindow->OnUpdate(timestep);

            // Temporary
            m_CurrentWindow = m_WindowBottom.get();
            m_CurrentWindow->FrameDrawOn();

            m_CurrentWindow->OnUpdate(timestep);

            m_ImGuiLayer->End();

            m_CurrentWindow->ScanHIDEvents();

            C3D_FrameEnd(0);
        }
    }
}