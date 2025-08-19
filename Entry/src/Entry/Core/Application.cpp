#include "etpch.h"
#include "Application.h"

#include <time.h>

#include "Input.h"
#include "Entry/Renderer/Renderer.h"
#include "Entry/Core/Timestep.h"
#include "Entry/Utils/PlatformUtils.h"

#include "Config.h"

namespace Entry
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        ET_PROFILE_FUNCTION();

#ifdef ET_PLATFORM_3DS
        Renderer::Init(); // Initialize graphics before windows for 3DS
#endif // ET_PLATFORM_3DS

        #ifdef ET_LOG_ENABLED
            Log::Init();
        #endif

        ET_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        // App Settings (temporary)
        std::string windowNames[] = { "Top", "Bottom" };
        uint32_t windowResolutions[2][2] = {{400, 240}, {320, 240}};
        int clearColor = 0x68B0D8FF;
        bool enableStereo3D = true;

        // Create Windows
        for (int i = 0; i < m_WindowCount; ++i)
        {
            ET_PROFILE_SCOPE("Create Window");
            WindowProps currentProps(windowNames[i], windowResolutions[i][0], windowResolutions[i][1], i, enableStereo3D);
            m_Windows[i] = Scope<Window>(Window::Create(currentProps));
            m_CurrentWindow = m_Windows[i].get();
            RenderCommand::SetClearColor(clearColor);

#ifdef ET_PLATFORM_WINDOWS
            if (i==0) Renderer::Init(); // Initialize graphics after windows for Windows
            m_CurrentWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));
#endif // ET_PLATFORM_WINDOWS

        }

#ifdef ET_PLATFORM_3DS
        // Bindevents to last screen (bottom screen atm)
        m_CurrentWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));
#endif // ET_PLATFORM_3DS

        {
            ET_PROFILE_SCOPE("Create ImGui Layer");
            m_ImGuiLayer = new Entry::ImGuiLayer();
            PushOverlay(m_ImGuiLayer, ET_WINDOW_BOTTOM);
        }
    }


    Application::~Application()
    {
    }

    /// <summary>
    /// PushLayer(Layer* layer, int window) adds a layer to the layer stack.
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="window"></param>
    void Application::PushLayer(Layer* layer, int window)
    {
        ET_PROFILE_FUNCTION();

        m_Windows[window]->PushLayer(layer);
        layer->OnAttach();
    }

    /// <summary>
    /// Pushes an overlay, which gets rendered over layers (i.e. GUI)
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="window"></param>
    void Application::PushOverlay(Layer* layer, int window)
    {
        ET_PROFILE_FUNCTION();

        m_Windows[window]->PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::Close()
    {
        m_Running = false;
    }

    void Application::OnEvent(Event& e)
    {
        ET_PROFILE_FUNCTION();

        //ET_CORE_TRACE("{0}", e);

        for (uint32_t i = 0; i < m_Windows.size(); ++i)
        {
            m_Windows[i]->OnEvent(e);
        }


#ifdef ET_PLATFORM_WINDOWS
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

#endif // ET_PLATFORM_WINDOWS
    }

    /// <summary>
    /// The main game loop.
    /// </summary>
    void Application::Run()
    {
        ET_PROFILE_FUNCTION();

#ifdef ET_PLATFORM_3DS
        while (aptMainLoop() && m_Running)
#endif // ET_PLATFORM_3DS
#ifdef ET_PLATFORM_WINDOWS
        while (m_Running)
#endif
        {
            ET_PROFILE_SCOPE("Run Loop");

            float time = Time::GetTime();
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            m_ImGuiLayer->Begin(timestep);
            for (uint32_t i = 0; i < m_WindowCount; ++i)
            {
                ET_PROFILE_SCOPE("Render Window");

                m_CurrentWindow = m_Windows[i].get();
                m_CurrentWindow->OnUpdate(timestep);
            }
            m_ImGuiLayer->End();
            
            {
                ET_PROFILE_SCOPE("ScanHIDEvents");
                m_CurrentWindow->ScanHIDEvents();
            }
            
            {
                ET_PROFILE_SCOPE("C3D_FrameEnd");
                m_CurrentWindow->FrameEnd();
            }
        }
    }

#ifdef ET_PLATFORM_WINDOWS

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }
    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        ET_PROFILE_FUNCTION();

        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight(), e.GetWindow());

        return false;
    }
#endif // ET_PLATFORM_WINDOWS

}