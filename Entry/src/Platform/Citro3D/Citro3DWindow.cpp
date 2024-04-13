#include "etpch.h"
#include "Citro3DWindow.h"

#include "Entry/Events/ApplicationEvent.h"
#include "Entry/Events/MouseEvent.h"
#include "Entry/Events/KeyEvent.h"

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
		m_Data.Stereo3D = false;

		C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

		ET_CORE_INFO("Creating screen {0} ({1}, {2})", props.Title, props.Width, props.Height);

		m_Window = C3D_RenderTargetCreate((int)props.Width, (int)props.Height, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
		C3D_RenderTargetSetOutput(m_Window, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
		
		if (m_Data.Stereo3D)
		{
			m_WindowR = C3D_RenderTargetCreate((int)props.Width, (int)props.Height, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
			C3D_RenderTargetSetOutput(m_WindowR, GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);
		}
	}

	void Citro3DWindow::Shutdown()
	{
		C3D_Fini();
		gfxExit();
	}

	void Citro3DWindow::OnUpdate()
	{
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C3D_RenderTargetClear(m_Window, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
			C3D_FrameDrawOn(m_Window);
		C3D_FrameEnd(0);

		hidScanInput();
	}

	void Citro3DWindow::SetVSync(bool enabled)
	{
	}

	bool Citro3DWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}