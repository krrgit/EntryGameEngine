#include "etpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/Citro3D/Citro3DShader.h"

namespace Entry {

    Shader* Shader::Create(const uint8_t* shbinData, u32 shBinSize)
    {

        switch (Renderer::GetAPI()) 
        {
            case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::Citro3D:  return new Citro3DShader((u32*)shbinData, shBinSize);
        }

        return nullptr;
    }
}