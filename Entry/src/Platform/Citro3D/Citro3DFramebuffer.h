#pragma once

#include "Entry/Renderer/Framebuffer.h"

#include <citro3d.h>
#include <citro2d.h>

namespace Entry {
	
	class Citro3DFramebuffer : public Framebuffer {
	public:
		Citro3DFramebuffer(const FramebufferSpecification& spec);
		virtual ~Citro3DFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override {}
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override { return 0; }
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override {}


		virtual void* GetColorAttachmentRendererID(uint32_t index = 0) const override { return m_ColorAttachment; }
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification;  };

	public:
		static C3D_RenderTarget* GetCurrentRenderTarget();
		static void SetCurrentRenderTarget(C3D_RenderTarget* target);

		static uint32_t GetClearColor();
		static void SetClearColor(uint32_t color);
	private:
		C3D_RenderTarget* m_RenderTarget;
		C3D_Tex* m_ColorAttachment;
		FramebufferSpecification m_Specification;

		static C3D_RenderTarget* s_CurrentRenderTarget;
	};
}