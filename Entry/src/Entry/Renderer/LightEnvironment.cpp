#include "etpch.h"
#include "LightEnvironment.h"

#include "Renderer.h"

#ifdef ET_PLATFORM_3DS
#include "Platform/Citro3D/Citro3DLightEnvironment.h"
#endif // ET_PLATFORM_3DS
#ifdef ET_PLATFORM_WINDOWS
#include "Platform/OpenGL/OpenGLLightEnvironment.h"
#endif

namespace Entry
{
	Ref<LightEnvironment> LightEnvironment::Create()
	{

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
		case RendererAPI::API::Citro3D:  return Ref<Citro3DLightEnvironment>(new Citro3DLightEnvironment());
#endif
#ifdef ET_PLATFORM_WINDOWS
		case RendererAPI::API::OpenGL:  return Ref<OpenGLLightEnvironment>(new OpenGLLightEnvironment());
#endif
		default: return nullptr;
		}

		return nullptr;
	}
}