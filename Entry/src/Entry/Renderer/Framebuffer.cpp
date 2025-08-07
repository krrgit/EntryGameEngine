#include "etpch.h"
#include "Framebuffer.h"

#include "Entry/Renderer/Renderer.h"

#include "Platform/Citro3D/Citro3DFramebuffer.h"

namespace Entry {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::Citro3D:  return Ref<Citro3DFramebuffer>(new Citro3DFramebuffer(spec));
		}

		return nullptr;
	}
}
