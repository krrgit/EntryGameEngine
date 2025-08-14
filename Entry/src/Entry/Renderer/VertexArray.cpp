#include "etpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "RendererAPI.h"

#ifdef ET_PLATFORM_3DS
#include "Platform/Citro3D/Citro3DVertexArray.h"
#endif

namespace Entry {

	Ref<VertexArray> VertexArray::Create()
	{
		ET_PROFILE_FUNCTION();

		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
		case RendererAPI::API::Citro3D:  return Ref<Citro3DVertexArray>(new Citro3DVertexArray());
#endif
		default: return nullptr;
		}

		return nullptr;
	}


}