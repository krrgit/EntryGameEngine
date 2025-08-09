#include "etpch.h"
#include "RendererAPI.h"

namespace Entry {
#ifdef ET_PLATFORM_3DS
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::Citro3D;
#endif
#ifdef ET_PLATFORM_WINDOWS
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
#endif
}