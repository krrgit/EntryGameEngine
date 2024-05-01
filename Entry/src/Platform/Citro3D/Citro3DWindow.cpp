#include "etpch.h"
#include "Citro3DWindow.h"

#include "Entry/Events/ApplicationEvent.h"
#include "Entry/Events/ScreenEvent.h"
#include "Entry/Events/KeyEvent.h"
#include "Entry/Events/CirclePadEvent.h"

#include "Platform/Citro3D/C2DPrepareLayer.h"


//#define CLEAR_COLOR 0x68B0D8FF
#define CLEAR_COLOR 0x68B0D8FF //0x191919FF

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

namespace Entry 
{
	Window* Window::Create(const WindowProps& props)
	{
		return new Citro3DWindow(props);
	}

	Citro3DWindow::Citro3DWindow(const WindowProps& props)
	{
		Init(props);
	}

	Citro3DWindow::~Citro3DWindow()
	{
		Shutdown();
	}

	void Citro3DWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Screen = props.Screen;
		m_Data.Stereo3D = false;

		ET_CORE_INFO("Create screen {0} ({1},{2})", props.Title, props.Width, props.Height);

		// C3D flips height and width (screen draws left to right)
		m_RenderTarget = C3D_RenderTargetCreate((int)props.Height, (int)props.Width, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
		C3D_RenderTargetSetOutput(m_RenderTarget, m_Data.Screen, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	}

	void Citro3DWindow::Shutdown()
	{
		C3D_Fini();
		gfxExit();
	}

	void Citro3DWindow::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Citro3DWindow::PushOverlay(Layer* layer)
	{
		// Add Layer to prepare 2D overlays
		if (!m_Data.has2D) {
			m_LayerStack.PushOverlay(new C2DPrepareLayer());
			m_Data.has2D = true;
		}

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Citro3DWindow::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Citro3DWindow::OnUpdate(Timestep ts)
	{
		for (Layer* layer : m_LayerStack)
			layer->OnUpdate(ts);

		for (Layer* layer : m_LayerStack)
			layer->OnImGuiRender();

		C2D_Flush();


	}

	void Citro3DWindow::ScanHIDEvents()
	{
		if (!hasEventCallback) return;

		hidScanInput();
		TriggerEvents();
	}

	void Citro3DWindow::FrameDrawOn()
	{
		C3D_RenderTargetClear(m_RenderTarget, C3D_CLEAR_ALL, m_ClearColor, 0);
		C3D_FrameDrawOn(m_RenderTarget);
		C2D_SceneTarget(m_RenderTarget);


		// Configure the first fragment shading substage to just pass through the vertex color
		// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
		C3D_TexEnv* env = C3D_GetTexEnv(0);
		C3D_TexEnvInit(env);
		C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR, GPU_PRIMARY_COLOR);
		C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);

		// Clear out the other texenvs
		C3D_TexEnvInit(C3D_GetTexEnv(1));
		C3D_TexEnvInit(C3D_GetTexEnv(2));
		C3D_TexEnvInit(C3D_GetTexEnv(3));
		C3D_TexEnvInit(C3D_GetTexEnv(4));
		C3D_TexEnvInit(C3D_GetTexEnv(5));
	}

	void Citro3DWindow::SetVSync(bool enabled)
	{
	}

	bool Citro3DWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void Citro3DWindow::TriggerEvents() {
		anyKeyPressed = hidKeysDown();
		anyKeyHeld = hidKeysHeld();
		anyKeyReleased = hidKeysUp();

		//Check if some of the keys are down, held or up
		uint8_t i;
		uint32_t keyCode;
		for (i = 0; i < ALL_KEYS_COUNT; i++)
		{
			keyCode = BIT(i);
			if (i == KEY_TOUCH) 
			{
				if (anyKeyPressed & keyCode)
				{
					hidTouchRead(&touchPos);
					ScreenTouchedEvent event(touchPos.px, touchPos.py);
					m_Data.EventCallback(event);

				} else if (anyKeyHeld & keyCode)
				{
					hidTouchRead(&touchPos);
					ScreenTouchedEvent event(touchPos.px, touchPos.py);
					m_Data.EventCallback(event);

				} else if (anyKeyReleased & keyCode)
				{
					ScreenReleasedEvent event(touchPos.px, touchPos.py);
					m_Data.EventCallback(event);
				}

				continue;
			}

			if (anyKeyPressed & keyCode)
			{
				KeyPressedEvent event(keyCode, 0);
				m_Data.EventCallback(event);
			}

			if (anyKeyHeld & keyCode) {
				KeyPressedEvent event(keyCode, 1);
				m_Data.EventCallback(event);
			}

			if (anyKeyReleased & keyCode) {
				KeyReleasedEvent event(keyCode);
				m_Data.EventCallback(event);
			}
		}

		hidCircleRead(&circlePadPos);
		float deadzone = 0.05f;
		if (sqrt(circlePadPos.dx * circlePadPos.dx + circlePadPos.dy * circlePadPos.dy) >= deadzone) {
			CirclePadEvent event(circlePadPos.dx, circlePadPos.dy);
			m_Data.EventCallback(event);
		}
	}
}