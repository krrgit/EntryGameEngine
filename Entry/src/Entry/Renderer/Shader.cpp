#include "etpch.h"
#include "Shader.h"

#include "Renderer.h"

#ifdef ET_PLATFORM_3DS
#include "Platform/Citro3D/Citro3DShader.h"
#endif

namespace Entry {

    Shader* Shader::Create(const uint8_t* shbinData, uint32_t shBinSize)
    {

        switch (Renderer::GetAPI()) 
        {
            case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
            case RendererAPI::API::Citro3D:  return new Citro3DShader((uint32_t*)shbinData, shBinSize);
#endif
            default: return nullptr;
        }

        return nullptr;
    }
}