#include "etpch.h"
#include "Buffer.h"

#include "Renderer.h"

#ifdef ET_PLATFORM_3DS
#include "Platform/Citro3D/Citro3DBuffer.h"
#endif // ET_PLATFORM_3DS
#ifdef ET_PLATFORM_WINDOWS
#include "Platform/OpenGL/OpenGLBuffer.h"
#endif // ET_PLATFORM_WINDOWS


namespace Entry
{
    VertexBuffer* VertexBuffer::Create(uint32_t size)
    {
        switch (Renderer::GetAPI()) {
            case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
            case RendererAPI::API::Citro3D:  return new Citro3DVertexBuffer(size);
#endif // ET_PLATFORM_3DS
#ifdef ET_PLATFORM_WINDOWS
            case RendererAPI::API::OpenGL:  return new OpenGLVertexBuffer(size);
#endif // ET_PLAFTFORM_WINDOWS

            default: return nullptr;
        }

        return nullptr;
    }

    VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
    {

        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
        case RendererAPI::API::Citro3D:  return new Citro3DVertexBuffer(vertices, size);
#endif
#ifdef ET_PLATFORM_WINDOWS
        case RendererAPI::API::OpenGL:  return new OpenGLVertexBuffer(vertices, size);
#endif // ET_PLAFTFORM_WINDOWS
        default: return nullptr;

        }

        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(uint16_t* indices, uint16_t count)
    {
        switch (Renderer::GetAPI()) {
            case RendererAPI::API::None:     return nullptr;
#ifdef ET_PLATFORM_3DS
            case RendererAPI::API::Citro3D:  return new Citro3DIndexBuffer(indices, count);
#endif
#ifdef ET_PLATFORM_WINDOWS
            case RendererAPI::API::OpenGL:  return new OpenGLIndexBuffer(indices, count);
#endif
            default: return nullptr;
        }

        return nullptr;
    }
}