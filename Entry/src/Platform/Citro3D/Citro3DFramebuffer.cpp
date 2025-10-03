#include "etpch.h"
#include "Citro3DFramebuffer.h"
#include "Entry/Core/Core.h"

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

namespace Entry {

	static C3D_RenderTarget* g_CurrentRenderTarget = nullptr;
	static uint32_t g_ClearColor = 0x68B0D8FF;

	Citro3DFramebuffer::Citro3DFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		ET_PROFILE_FUNCTION();
		m_RenderTarget = C3D_RenderTargetCreate((int)spec.Height, (int)spec.Width, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
		C3D_RenderTargetSetOutput(m_RenderTarget, (gfxScreen_t)spec.Screen, (gfx3dSide_t)spec.Side, DISPLAY_TRANSFER_FLAGS);
	}

	Citro3DFramebuffer::~Citro3DFramebuffer()
	{
		ET_PROFILE_FUNCTION();
		C3D_RenderTargetDelete(m_RenderTarget);
	}

	void Citro3DFramebuffer::Bind()
	{
		ET_PROFILE_FUNCTION();
		C3D_FrameDrawOn(m_RenderTarget);
		C2D_SceneTarget(m_RenderTarget);

		g_CurrentRenderTarget = m_RenderTarget;
	}
	void Citro3DFramebuffer::Unbind()
	{
		//C2D_Flush();
		//C3D_SetFrameBuf(0);
		g_CurrentRenderTarget = nullptr;
	}

	void Citro3DFramebuffer::Invalidate() 
	{
		// TODO ?
	}

	C3D_RenderTarget* Citro3DFramebuffer::GetCurrentRenderTarget()
	{
		return g_CurrentRenderTarget;
	}
	void Citro3DFramebuffer::SetCurrentRenderTarget(C3D_RenderTarget* target)
	{
		g_CurrentRenderTarget = target;
	}
	uint32_t Citro3DFramebuffer::GetClearColor()
	{
		return g_ClearColor;
	}
	void Citro3DFramebuffer::SetClearColor(uint32_t color)
	{
		g_ClearColor = color;
	}

}