#pragma once

#include "Entry/Core/Core.h"
#include <vector>

namespace Entry 
{

	typedef enum
	{
		GFX_TOP = 0,    ///< Top screen
		GFX_BOTTOM = 1,
		GFX_NONE = 2,
	} ET_GFX_SCREEN;

	typedef enum
	{
		GFX_LEFT = 0, ///< Left eye framebuffer
		GFX_RIGHT = 1, ///< Right eye framebuffer
	} ET_GFX_3D_SIDE;

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
		ET_GFX_SCREEN Screen = ET_GFX_SCREEN::GFX_TOP;
		ET_GFX_3D_SIDE Side = ET_GFX_3D_SIDE::GFX_LEFT;

		bool SwapChainTarget = false;
	};

	class Framebuffer {
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;
		
		virtual void* GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);


	};
}