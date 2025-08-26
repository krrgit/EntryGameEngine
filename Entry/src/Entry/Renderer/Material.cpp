#include "etpch.h"
#include "Material.h"
#include "Renderer.h"

#ifdef ET_PLATFORM_3DS
#include "Platform/Citro3D/Citro3DMaterial.h"
#endif // ET_PLATFORM_3DS
#ifdef ET_PLATFORM_WINDOWS	
#include "Platform/OpenGL/OpenGLMaterial.h"
#endif //ET_PLATFORM_WINDOWS


namespace Entry {

	Ref<Material> Material::Create(MaterialProps& props)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
		case RendererAPI::API::Citro3D:  return Ref<Citro3DMaterial>(new Citro3DMaterial(props));
#endif
#ifdef ET_PLATFORM_WINDOWS
		case RendererAPI::API::OpenGL:  return Ref<OpenGLMaterial>(new OpenGLMaterial(props));
#endif
		default: return nullptr;
		}

		return nullptr;
	}

}