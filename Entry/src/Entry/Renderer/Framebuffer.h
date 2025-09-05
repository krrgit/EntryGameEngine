#pragma once

#include "Entry/Core/Core.h"
#include <vector>

namespace Entry {
	enum class FramebufferTextureFormat
	{
		None = 0,

		//Color
		RGBA8,
		RED_INTEGER,

		// Depth/Stencil
		DEPTH24STENCIL8,
		
		// Default
		Depth = DEPTH24STENCIL8 
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
		: TextureFormat(format)
		{}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification {
		uint32_t Width, Height;
		FramebufferAttachmentSpecification Attachments;
		int Samples = 1;

		bool SwapChainTarget = false;
	};

	class Framebuffer {
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;
		
		virtual void* GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);


	};
}