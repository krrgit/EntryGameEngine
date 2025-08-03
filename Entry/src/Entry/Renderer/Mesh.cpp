#include "etpch.h"
#include "Mesh.h"

#include "Renderer.h"
#include "Platform/Citro3D/Citro3DMesh.h"


namespace Entry {
	Ref<Mesh> Mesh::Create(const std::string& path)
	{

		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::Citro3D:  return Ref<Citro3DMesh>(new Citro3DMesh(path));
		}

		return nullptr;
	}
}