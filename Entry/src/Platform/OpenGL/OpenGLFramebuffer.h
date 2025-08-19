#pragma once
#include "Entry/Renderer/Framebuffer.h"

namespace Entry{
    class OpenGLFramebuffer : public Framebuffer
    {
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void* GetColorAttachmentRendererID() const override { return (void*)m_ColorAttachment; }
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; };

	private:
		uint32_t m_RendererID;
		uint32_t m_ColorAttachment, m_DepthAttachment;
		FramebufferSpecification m_Specification;
	
    };
}

