#include "etpch.h"
#include "Application.h"

#include "Input.h"
#include "Entry/Renderer/Renderer.h"
#include "Entry/Core/Timestep.h"
#include <time.h>
#include <memory>

#include "Config.h"

namespace Entry
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;


    Application::Application()
    {
        ET_PROFILE_FUNCTION();

        Renderer::Init();

        #ifdef ET_LOG_ENABLED
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

        m_Windows.resize(m_WindowCount);
        std::string windowNames[] = { "Top", "Bottom" };
        uint32_t windowResolutions[2][2] = {{400, 240}, {320, 240}};
        int clearColor = 0x68B0D8FF;
        bool enableStereo3D = false;

        for (int i = 0; i < m_WindowCount; ++i)
        {
            ET_PROFILE_SCOPE("Create Window");
            WindowProps currentProps(windowNames[i], windowResolutions[i][0], windowResolutions[i][1], i, enableStereo3D);
            m_Windows[i] = Scope<Window>(Window::Create(currentProps));
            m_CurrentWindow = m_Windows[i].get();
            RenderCommand::SetClearColor(clearColor);
        }

        // Bind events to last screen (bottom screen atm)
        m_CurrentWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));

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

        for (uint32_t i = 0; i < m_Windows.size(); ++i)
        {
            m_Windows[i]->OnEvent(e);
        }

    }

    /// <summary>
    /// The main game loop.
    /// </summary>
    void Application::Run()
    {
        ET_PROFILE_FUNCTION();

        while (aptMainLoop() && m_Running) {
            ET_PROFILE_SCOPE("Run Loop");

            osTickCounterUpdate(&m_FrameTime);
            Timestep timestep = osTickCounterRead(&m_FrameTime) * 0.001f;
            osTickCounterStart(&m_FrameTime);

            m_ImGuiLayer->Begin(timestep);
            for (uint32_t i = 0; i < m_WindowCount; ++i)
            {
                ET_PROFILE_SCOPE("Render Window");

                m_CurrentWindow = m_Windows[i].get();
                m_CurrentWindow->OnUpdate(timestep);
                m_CurrentWindow->ScanHIDEvents();
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
}