#include "etpch.h"
#include "Citro3DWindow.h"

#include "Entry/Events/ApplicationEvent.h"
#include "Entry/Events/ButtonEvent.h"
#include "Entry/Events/ScreenEvent.h"
#include "Entry/Events/CirclePadEvent.h"
#include "Entry/Events/Slider3DEvent.h"

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
		ET_PROFILE_FUNCTION();

		Init(props);
	}

	Citro3DWindow::~Citro3DWindow()
	{
		ET_PROFILE_FUNCTION();

		Shutdown();
	}

	void Citro3DWindow::Init(const WindowProps& props)
	{
		// DO NOT PROFILE THIS FUNCTION. IT CAUSES ISSUES FOR SOME REASON.

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Screen = (gfxScreen_t)props.Screen;
		m_Data.Stereo3D = props.Stereo3D && (m_Data.Screen == GFX_TOP);

		ET_CORE_INFO("Create screen {0} ({1},{2})", props.Title, props.Width, props.Height);

		// C3D flips height and width (screen draws left to right)
		m_RenderTarget = C3D_RenderTargetCreate((int)props.Height, (int)props.Width, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
		C3D_RenderTargetSetOutput(m_RenderTarget, m_Data.Screen, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

		// 2nd render target for 3D
		if (m_Data.Stereo3D)
		{
			m_RenderTargetR = C3D_RenderTargetCreate((int)props.Height, (int)props.Width, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
			C3D_RenderTargetSetOutput(m_RenderTargetR, m_Data.Screen, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);
		}
	}

	void Citro3DWindow::Shutdown()
	{
		ET_PROFILE_FUNCTION();

		C3D_Fini();
		gfxExit();
	}

	void Citro3DWindow::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Citro3DWindow::PushOverlay(Layer* layer)
	{
		// Add Layer to prepare 2D overlays
		if (!m_Data.has2D) {
			m_LayerStack.PushOverlay(new C2DPrepareLayer());
			m_Data.has2D = true;
		}

		m_LayerStack.PushOverlay(layer);
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
		ET_PROFILE_FUNCTION();

		FrameDrawOn(GFX_LEFT);
		LayerStackOnUpdate(ts);

		{
			ET_PROFILE_SCOPE("LayerStack OnImGuiRender");
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
		}
		{
			ET_PROFILE_SCOPE("C2D_Flush");
			C2D_Flush();
		}

		// Don't draw right side
		if (!m_Data.Stereo3D || m_Slider3DState <= 0.0f) return;
		
		FrameDrawOn(GFX_RIGHT);
		LayerStackOnUpdate(ts, (uint16_t) GFX_RIGHT);
	}

	void Citro3DWindow::LayerStackOnUpdate(Timestep ts, uint16_t screenSide)
	{
		{
			ET_PROFILE_SCOPE("LayerStack OnUpdate");
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(ts, screenSide);
		}
	}

	void Citro3DWindow::ScanHIDEvents()
	{
		if (!hasEventCallback) return;

		TriggerEvents();
	}

	void Citro3DWindow::FrameDrawOn(gfx3dSide_t side)
	{
		ET_PROFILE_FUNCTION();

		if (side == GFX_LEFT)
		{	
			// Begin Frame when drawing top left screen
			if (m_Data.Screen == GFX_TOP) 
			{
			
				C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

				// Reset TexEnvs
				// Pass through vertex color
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

			C3D_RenderTargetClear(m_RenderTarget, C3D_CLEAR_ALL, m_ClearColor, 0);
			C3D_FrameDrawOn(m_RenderTarget);
			C2D_SceneTarget(m_RenderTarget);
		}
		else if (side == GFX_RIGHT)
		{	
			// Draw On Right Screen (Top only)
			C3D_RenderTargetClear(m_RenderTargetR, C3D_CLEAR_ALL, m_ClearColor, 0);
			C3D_FrameDrawOn(m_RenderTargetR);
			C2D_SceneTarget(m_RenderTargetR);
		}
	}

	void Citro3DWindow::FrameEnd()
	{
		C3D_FrameEnd(0);
	}

	void Citro3DWindow::SetVSync(bool enabled)
	{
		ET_PROFILE_FUNCTION();

	}

	bool Citro3DWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void Citro3DWindow::TriggerEvents() {
		hidScanInput();

		anyButtonPressed = hidKeysDown();
		anyButtonHeld = hidKeysHeld();
		anyButtonReleased = hidKeysUp();
		m_Slider3DState = osGet3DSliderState();

		//Check if some of the keys are down, held or up
		uint8_t i;
		uint32_t keyCode;
		for (i = 0; i < ALL_BUTTONS_COUNT; i++)
		{
			keyCode = BIT(i);
			if (keyCode == KEY_TOUCH) 
			{
				if (anyButtonPressed & keyCode)
				{
					hidTouchRead(&touchPos);
					ScreenTouchedEvent event(touchPos.px, touchPos.py);
					m_Data.EventCallback(event);

				} else if (anyButtonHeld & keyCode)
				{
					hidTouchRead(&touchPos);
					ScreenTouchedEvent event(touchPos.px, touchPos.py);
					m_Data.EventCallback(event);

				} else if (anyButtonReleased & keyCode)
				{
					ScreenReleasedEvent event(touchPos.px, touchPos.py);
					m_Data.EventCallback(event);
				}

				continue;
			}

			if (anyButtonPressed & keyCode)
			{
				ButtonPressedEvent event((KeyCode)keyCode, 0);
				m_Data.EventCallback(event);
			}

			if (anyButtonHeld & keyCode) {
				ButtonPressedEvent event((KeyCode)keyCode, 1);
				m_Data.EventCallback(event);
			}

			if (anyButtonReleased & keyCode) {
				ButtonReleasedEvent event((KeyCode)keyCode);
				m_Data.EventCallback(event);
			}
		}

		hidCircleRead(&circlePadPos);
		float deadzone = 0.05f;
		if (sqrt(circlePadPos.dx * circlePadPos.dx + circlePadPos.dy * circlePadPos.dy) >= deadzone) {
			CirclePadEvent event(circlePadPos.dx, circlePadPos.dy);
			m_Data.EventCallback(event);
		}

		if (m_Slider3DState != m_Slider3DStatePrev) {
			Slider3DEvent event(m_Slider3DState);
			m_Data.EventCallback(event);

			m_Slider3DStatePrev = m_Slider3DState;
		}
	}
}