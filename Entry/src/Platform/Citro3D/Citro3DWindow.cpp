#include "etpch.h"
#include "Citro3DWindow.h"

#include "Entry/Events/ApplicationEvent.h"
#include "Entry/Events/ScreenEvent.h"
#include "Entry/Events/KeyEvent.h"
#include "Entry/Events/CirclePadEvent.h"

#define CLEAR_COLOR 0x68B0D8FF

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
		m_Data.Screen = (gfxScreen_t)props.Screen;
		m_Data.Stereo3D = false;

		C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

		ET_CORE_INFO("Creating screen {0} ({1}, {2})", props.Title, props.Width, props.Height);

		// C3D flips height and width?
		m_RenderTarget = C3D_RenderTargetCreate((int)props.Height, (int)props.Width, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
		C3D_RenderTargetSetOutput(m_RenderTarget, m_Data.Screen, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
		
		if (m_Data.Screen == GFX_TOP && m_Data.Stereo3D)
		{
			m_RenderTargetR = C3D_RenderTargetCreate((int)props.Height, (int)props.Width, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
			C3D_RenderTargetSetOutput(m_RenderTargetR, m_Data.Screen, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);
		}
	}

	void Citro3DWindow::Shutdown()
	{
		C3D_Fini();
		gfxExit();
	}

	void Citro3DWindow::OnUpdate()
	{
		hidScanInput();
		TriggerEvents();
	}

	void Citro3DWindow::FrameBegin()
	{
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C3D_RenderTargetClear(m_RenderTarget, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
		C3D_FrameDrawOn(m_RenderTarget);
	}

	void Citro3DWindow::FrameEnd() 
	{
		C3D_FrameEnd(0);
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
		for (i = 0; i < ALL_KEYS_COUNT; i++)
		{
			if (i == KEY_TOUCH) 
			{
				if (anyKeyPressed & BIT(i))
				{
					hidTouchRead(&touchPos);
					ScreenTouchedEvent event(touchPos.px, touchPos.py);
					m_Data.EventCallback(event);

				} else if (anyKeyHeld & BIT(i)) 
				{
					hidTouchRead(&touchPos);
					ScreenTouchedEvent event(touchPos.px, touchPos.py);
					m_Data.EventCallback(event);

				} else if (anyKeyReleased & BIT(i))
				{
					ScreenReleasedEvent event(touchPos.px, touchPos.py);
					m_Data.EventCallback(event);
				}

				continue;
			}

			if (anyKeyPressed & BIT(i))
			{
				KeyPressedEvent event(i, 0);
				m_Data.EventCallback(event);
			}

			if (anyKeyHeld & BIT(i)) {
				KeyPressedEvent event(i, 1);
				m_Data.EventCallback(event);
			}

			if (anyKeyReleased & BIT(i)) {
				KeyReleasedEvent event(i);
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