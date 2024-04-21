#include "etpch.h"
#include "VertexArray.h"
#include "Citro3DVertexArray.h"

#include "Renderer.h"
#include "Platform/Citro3D/Citro3DVertexArray.h"

namespace Entry {

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::Citro3D:  return new Citro3DVertexArray();
		}

		return nullptr;
	}


}