#include "etpch.h"
#include "Shader.h"

#include "Renderer.h"

#ifdef ET_PLATFORM_3DS
#include "Platform/Citro3D/Citro3DShader.h"
#endif
#ifdef ET_PLATFORM_WINDOWS
#include "Platform/OpenGL/OpenGLShader.h"
#endif // ET_PLATFORM_WINDOWS



namespace Entry {

    Shader* Shader::Create(const uint8_t* shbinData, uint32_t shBinSize)
    {

        switch (Renderer::GetAPI()) 
        {
            case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
            case RendererAPI::API::Citro3D:  return new Citro3DShader((uint32_t*)shbinData, shBinSize);
#endif
#ifdef ET_PLATFORM_WINDOWS
            case RendererAPI::API::OpenGL:  ET_CORE_ASSERT(false, "Shader::Create(const uint8_t* shbinData, uint32_t shBinSize) is Citro3D only!"); return nullptr;
#endif

            default: return nullptr;
        }

        return nullptr;
    }
    Shader* Shader::Create(const std::string& filepath)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
        case RendererAPI::API::Citro3D:  ET_CORE_ASSERT(false, "RendererAPI::Citro3D creat from filepath currently not supported!"); return nullptr;
#endif
#ifdef ET_PLATFORM_WINDOWS
        case RendererAPI::API::OpenGL: return new OpenGLShader(filepath);
#endif // ET_PLATFORM_WINDOWS

        default: return nullptr;
        }

        return nullptr;
    }
}