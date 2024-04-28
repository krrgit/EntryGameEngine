#include "etpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/Citro3D/Citro3DShader.h"

namespace Entry {

    Shader* Shader::Create(int src_id)
    {

        switch (Renderer::GetAPI()) 
        {
            case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::Citro3D:  return new Citro3DShader(src_id);
        }

        return nullptr;
    }
}