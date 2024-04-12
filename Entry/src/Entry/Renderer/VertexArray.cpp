#include "etpch.h"
#include "VertexArray.h"
#include "Citro3DVertexArray.h"

#include "Renderer.h"

namespace Entry {
    VertexArray* VertexArray::Create()
    {
        switch(Renderer::GetAPI()) {
            case RendererAPI::None:     return nullptr;
            case RendererAPI::Citro3D:  return new Citro3DVertexArray();
        }

        return nullptr;
    }
}