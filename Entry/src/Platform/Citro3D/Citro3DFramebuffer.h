#pragma once

#include "Entry/Renderer/Framebuffer.h"

#include <citro3d.h>


namespace Entry {
	class Citro3DFramebuffer : public Framebuffer {
	public:
		Citro3DFramebuffer(const FramebufferSpecification& spec);
		virtual ~Citro3DFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void* GetColorAttachmentRendererID() const override { return m_ColorAttachment; }
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification;  };

	private:
		C3D_FrameBuf* m_FrameBuf;
		C3D_Tex* m_ColorAttachment;
		FramebufferSpecification m_Specification;
	};
}