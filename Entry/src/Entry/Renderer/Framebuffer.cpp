#include "etpch.h"
#include "Framebuffer.h"

#include "Entry/Renderer/Renderer.h"

#ifdef ET_PLATFORM_3DS
#include "Platform/Citro3D/Citro3DFramebuffer.h"
#endif

#ifdef ET_PLATFORM_WINDOWS
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#endif

namespace Entry {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
			case RendererAPI::API::Citro3D:  return Ref<Citro3DFramebuffer>(new Citro3DFramebuffer(spec));
#endif
#ifdef ET_PLATFORM_WINDOWS
			case RendererAPI::API::OpenGL:  return Ref<OpenGLFramebuffer>(new OpenGLFramebuffer(spec));
#endif
			default: return nullptr;
		}

		return nullptr;
	}
}
