#include "etpch.h"
#include "RenderCommand.h"

#include "Platform/Citro3D/Citro3DRendererAPI.h"

namespace Entry {
// Defined in Makefile
#ifdef ET_PLATFORM_3DS
	RendererAPI* RenderCommand::s_RendererAPI = new Citro3DRendererAPI;
#endif // ET_PLATFORM_3DS

// Defined in premake5.lua
#ifdef ET_PLATFORM_WINDOWS
	//RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
#endif // ET_PLATFORM_WINDOWS

}