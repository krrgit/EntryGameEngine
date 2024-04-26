#include "etpch.h"
#include "Application.h"

#include "Input.h"
#include "Entry/Renderer/Renderer.h"


#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

namespace Entry
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        gfxInitDefault();
        Log::Init();

        ET_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
        C2D_Init(C2D_DEFAULT_MAX_OBJECTS);

        WindowProps topProps("Top", 400, 240, GFX_TOP);
        m_WindowTop = std::unique_ptr<Window>(Window::Create(topProps));
        m_CurrentWindow = m_WindowTop.get();
        RenderCommand::SetClearColor(0x68B0D8FF);

        WindowProps props("Bottom", 320, 240, GFX_BOTTOM);
        m_WindowBottom = std::unique_ptr<Window>(Window::Create(props));
        m_WindowBottom->SetEventCallback(BIND_EVENT_FN(OnEvent));
        m_CurrentWindow = m_WindowBottom.get();
        RenderCommand::SetClearColor(0x191919FF);

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

            m_CurrentWindow = m_WindowTop.get();
            m_CurrentWindow->FrameDrawOn();

            m_CurrentWindow->OnUpdate();

            // Temporary
            m_CurrentWindow = m_WindowBottom.get();
            m_CurrentWindow->FrameDrawOn();

            m_CurrentWindow->OnUpdate();

            C3D_FrameEnd(0);
        }
    }
}