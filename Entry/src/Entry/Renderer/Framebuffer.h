#pragma once

#include "Entry/Core/Core.h"

namespace Entry {

	struct FramebufferSpecification {
		uint32_t Width, Height;

		bool SwapChainTarget = false;
	};

	class Framebuffer {
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;
		
		virtual void* GetColorAttachmentRendererID() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);


	};
}