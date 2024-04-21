#include "etpch.h"
#include "RenderCommand.h"

#include "Platform/Citro3D/Citro3DRendererAPI.h"

namespace Entry {
	RendererAPI* RenderCommand::s_RendererAPI = new Citro3DRendererAPI;
}