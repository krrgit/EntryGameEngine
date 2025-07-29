#include "etpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "RendererAPI.h"
#include "Platform/Citro3D/Citro3DVertexArray.h"

namespace Entry {

	Ref<VertexArray> VertexArray::Create()
	{
		ET_PROFILE_FUNCTION();

		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::Citro3D:  return Ref<Citro3DVertexArray>(new Citro3DVertexArray());
		}

		return nullptr;
	}


}