#include "etpch.h"
#include "Model.h"

#include "Renderer.h"

#include "Platform/Citro3D/Citro3DModel.h"
#include "Platform/OpenGL/OpenGLModel.h"

namespace Entry {
	Ref<Model> Model::Create(const std::string& path)
	{

		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
		case RendererAPI::API::Citro3D:  return Ref<Citro3DModel>(new Citro3DModel(path));
#endif
#ifdef ET_PLATFORM_WINDOWS
		case RendererAPI::API::OpenGL:  return Ref<OpenGLModel>(new OpenGLModel(path));
#endif
			default: return nullptr;	
		}

		return nullptr;
	}
}