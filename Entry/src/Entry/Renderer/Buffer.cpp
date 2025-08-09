#include "etpch.h"
#include "Buffer.h"

#include "Renderer.h"

#ifdef ET_PLATFORM_3DS
#include "Platform/Citro3D/Citro3DBuffer.h"
#endif // ET_PLATFORM_3DS

namespace Entry
{
    VertexBuffer* VertexBuffer::Create(uint32_t size)
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
#ifdef ET_PLATFORM_3DS
        case RendererAPI::API::Citro3D:  return new Citro3DVertexBuffer(size);
#endif // ET_PLATFORM_3DS
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
        }

        return nullptr;
    }
}