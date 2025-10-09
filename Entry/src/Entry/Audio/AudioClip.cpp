#include "etpch.h"
#include "Entry/Renderer/Renderer.h"

#include "AudioClip.h"

#ifdef ET_PLATFORM_3DS
#include "Platform/N3DS/N3DSAudioClip.h"
#endif // ET_PLATFORM_3DS
#ifdef ET_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsAudioClip.h"
#endif // ET_PLATFORM_WINDOWS


namespace Entry
{
	Ref<AudioClip> AudioClip::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
		case RendererAPI::API::Citro3D:  return Ref<N3DSAudioClip>(new N3DSAudioClip(path));
#endif
#ifdef ET_PLATFORM_WINDOWS
		case RendererAPI::API::OpenGL:  return Ref<WindowsAudioClip>(new WindowsAudioClip(path));
#endif
		default: return nullptr;
		}

		return nullptr;
	}
}