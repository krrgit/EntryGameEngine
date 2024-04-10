#include "Buffer.h"
#include "Renderer.h"
#include "Citro3DBuffer.h"

namespace Entry
{
    
    VertexBuffer* VertexBuffer::Create(float *vertices, uint32_t size)
    {
        switch(Renderer::GetAPI()) {
            case RendererAPI::None:     return nullptr;
            case RendererAPI::Citro3D:  return new Citro3DVertexBuffer(vertices, size);
        }

        return nullptr;
    }

    IndexBuffer *IndexBuffer::Create(uint16_t *indices, uint32_t size)
    {
        switch(Renderer::GetAPI()) {
            case RendererAPI::None:     return nullptr;
            case RendererAPI::Citro3D:  return new Citro3DIndexBuffer(indices, size);
        }

        return nullptr;
    }
}