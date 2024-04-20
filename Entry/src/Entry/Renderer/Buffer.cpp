#include "etpch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "Platform/Citro3D/Citro3DBuffer.h"

namespace Entry
{
    VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
    {

        switch (Renderer::GetAPI()) {
        case RendererAPI::None:     ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::Citro3D:  return new Citro3DVertexBuffer(vertices, size);
        }

        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(uint16_t* indices, uint16_t count)
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::None:     return nullptr;
        case RendererAPI::Citro3D:  return new Citro3DIndexBuffer(indices, count);
        }

        return nullptr;
    }
}