#include "Citro3DFramebuffer.h"
#include "Citro3DWindow.h"


namespace Entry {

	Citro3DFramebuffer::Citro3DFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		// TODO ?
		// Can't figure out how to render framebuffer to texture (run into issues in engine, works in standalone project)
	}

	Citro3DFramebuffer::~Citro3DFramebuffer()
	{
	}

	void Citro3DFramebuffer::Bind()
	{
	}
	void Citro3DFramebuffer::Unbind()
	{
		//C3D_SetFrameBuf(0);
	}

	void Citro3DFramebuffer::Invalidate() 
	{
		// TODO ?
	}

}