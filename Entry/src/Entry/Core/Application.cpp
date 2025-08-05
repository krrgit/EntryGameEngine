#include "etpch.h"
#include "Application.h"

#include "Input.h"
#include "Entry/Renderer/Renderer.h"
#include "Entry/Core/Timestep.h"
#include <time.h>
#include <memory>

#define DEBUG

#ifdef DEBUG
    #define ET_LOG_CONSOLE
#endif // DEBUG




namespace Entry
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        ET_PROFILE_FUNCTION();

        Renderer::Init();

        #ifdef ET_LOG_CONSOLE
            Log::Init();
        #endif

        //// romfs
        //Result rc = romfsInit();
        //if (rc)
        //    printf("romfsInit: %08lX\n", rc);
        //else
        //{
        //    printf("romfs Init Successful!\n");
        //}

        ET_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        {
            ET_PROFILE_SCOPE("Create Top Window");
            WindowProps topProps("Top", 400, 240, GFX_TOP);
            m_WindowTop = Scope<Window>(Window::Create(topProps));
            m_CurrentWindow = m_WindowTop.get();
            RenderCommand::SetClearColor(0x68B0D8FF);
        }
        {

            ET_PROFILE_SCOPE("Create Bottom Window");
            WindowProps props("Bottom", 320, 240, GFX_BOTTOM);
            m_WindowBottom = Scope<Window>(Window::Create(props));
            m_WindowBottom->SetEventCallback(BIND_EVENT_FN(OnEvent));
            m_CurrentWindow = m_WindowBottom.get();
            RenderCommand::SetClearColor(0x252525FF);
        }

        {
            ET_PROFILE_SCOPE("Create ImGui Layer");
            m_ImGuiLayer = new Entry::ImGuiLayer();
            PushOverlay(m_ImGuiLayer, ET_WINDOW_BOTTOM);
        }

        osTickCounterStart(&m_FrameTime);
    }


    Application::~Application()
    {
    }

    void Application::PushLayer(Layer* layer, int window)
    {
        ET_PROFILE_FUNCTION();

        if (window == ET_WINDOW_TOP) 
        {
            m_WindowTop->PushLayer(layer);
            layer->OnAttach();
        }
        else if (window == ET_WINDOW_BOTTOM) 
        {
            m_WindowBottom->PushLayer(layer);
            layer->OnAttach();
        }

    }

    void Application::PushOverlay(Layer* layer, int window)
    {
        ET_PROFILE_FUNCTION();

        if (window == ET_WINDOW_TOP) 
        {
            m_WindowTop->PushOverlay(layer);
            layer->OnAttach();
        }
        else if (window == ET_WINDOW_BOTTOM) 
        {
            m_WindowBottom->PushOverlay(layer);
            layer->OnAttach();
        }
    }

    void Application::Close()
    {
        m_Running = false;
    }

    void Application::OnEvent(Event& e)
    {
        ET_PROFILE_FUNCTION();

        m_WindowTop->OnEvent(e);
        m_WindowBottom->OnEvent(e);
    }

    void Application::Run()
    {
        ET_PROFILE_FUNCTION();

        while (aptMainLoop() && m_Running) {
            ET_PROFILE_SCOPE("Run Loop");

            {
                ET_PROFILE_SCOPE("C3D_FrameBegin");

                C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
            }

            osTickCounterUpdate(&m_FrameTime);
            Timestep timestep = osTickCounterRead(&m_FrameTime) * 0.001f;
            osTickCounterStart(&m_FrameTime);

            m_ImGuiLayer->Begin(timestep);
            {
                ET_PROFILE_SCOPE("Render Top Window");

                m_CurrentWindow = m_WindowTop.get();
                m_CurrentWindow->FrameDrawOn();

                m_CurrentWindow->OnUpdate(timestep);
            }

            // Temporary
            {
                ET_PROFILE_SCOPE("Render Bottom Window");

                m_CurrentWindow = m_WindowBottom.get();
                m_CurrentWindow->FrameDrawOn();

                m_CurrentWindow->OnUpdate(timestep);
            }

            m_ImGuiLayer->End();
            
            {
                ET_PROFILE_SCOPE("ScanHIDEvents");

                m_CurrentWindow->ScanHIDEvents();
            }
            
            {
                ET_PROFILE_SCOPE("C3D_FrameEnd");

                C3D_FrameEnd(0);
            }
        }
    }
}