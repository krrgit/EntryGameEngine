#include "etpch.h"
#include "Texture.h"

#include "Renderer.h"
#ifdef ET_PLATFORM_3DS
	#include "Platform/Citro3D/Citro3DTexture.h"
#endif
namespace Entry {
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height) 
	{

		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
		case RendererAPI::API::Citro3D:  return Ref<Citro3DTexture2D>(new Citro3DTexture2D(width, height));
#endif
		}

		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const void* data, size_t size)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
		case RendererAPI::API::Citro3D:  return Ref<Citro3DTexture2D>(new Citro3DTexture2D(data, size));
#endif
		}

		return nullptr;
	}
}