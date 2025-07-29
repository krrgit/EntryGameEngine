#include "etpch.h"
#include "Citro3DVertexArray.h"
#include "Entry/Renderer/Buffer.h"
#include <citro3d.h>

namespace Entry
{
    static GPU_FORMATS ShaderDataTypeToCitro3DDataType(ShaderDataType type)
    {
        switch (type) {
        case ShaderDataType::Float:     return GPU_FLOAT;
        case ShaderDataType::Float2:    return GPU_FLOAT;
        case ShaderDataType::Float3:    return GPU_FLOAT;
        case ShaderDataType::Float4:    return GPU_FLOAT;
        case ShaderDataType::Mat3:      return GPU_FLOAT;
        case ShaderDataType::Mat4:      return GPU_FLOAT;
        case ShaderDataType::Int:       return GPU_SHORT;
        case ShaderDataType::Int2:      return GPU_SHORT;
        case ShaderDataType::Int3:      return GPU_SHORT;
        case ShaderDataType::Int4:      return GPU_SHORT;
        case ShaderDataType::Bool:      return GPU_BYTE;
        default:
            break;
        }

        //ET_CORE_ERROR("Unknown ShaderDataType!");
        return GPU_BYTE;
    }

    Citro3DVertexArray::Citro3DVertexArray() {
        ET_PROFILE_FUNCTION();

        AttrInfo_Init(&m_AttrInfo);
    }

    void Citro3DVertexArray::Bind() {
        ET_PROFILE_FUNCTION();

        C3D_SetAttrInfo(&m_AttrInfo);
        m_VertexBuffers.front()->Bind();
    }

    void Citro3DVertexArray::Unbind() const {
        ET_PROFILE_FUNCTION();

        C3D_SetAttrInfo(0);
    }

    const void Citro3DVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
        ET_PROFILE_FUNCTION();

        ET_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!");

        //C3D_SetAttrInfo(&m_attrInfo);
        //vertexBuffer->Bind();

        uint32_t index = 0;
        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout) {
            AttrInfo_AddLoader(&m_AttrInfo,
                index++,
                ShaderDataTypeToCitro3DDataType(element.Type),
                element.GetComponentCount());
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    const void Citro3DVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
        ET_PROFILE_FUNCTION();

        m_IndexBuffer = indexBuffer;
    }
}