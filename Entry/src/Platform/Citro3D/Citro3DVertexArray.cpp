#include "etpch.h"
#include "Citro3DVertexArray.h"

namespace Entry
{
    static GPU_FORMATS ShaderDataTypeToCitro3DDataType(ShaderDataType type) 
    {
        switch(type) {
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
        attrInfo = C3D_GetAttrInfo();
        AttrInfo_Init(attrInfo);
    }

    void Citro3DVertexArray::Bind() const {
        C3D_BufInfo* bufInfo = C3D_GetBufInfo();
        BufInfo_Init(bufInfo);

        m_VertexBuffers.front()->Bind(bufInfo);
    }

    void Citro3DVertexArray::Unbind() const
    {
    }

    void Citro3DVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer)
    {
        if (vertexBuffer->GetLayout().GetElements().size() == 0) {
            //ET_CORE_ERROR("Vertex buffer has no layout!");
        }

        // vertexBuffer->Bind(bufInfo);
        int32_t index = 0;
        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout) {
            AttrInfo_AddLoader(attrInfo, 
                index, 
                ShaderDataTypeToCitro3DDataType(element.Type), 
                element.GetComponentCount());
            index++;
        }
	
        m_VertexBuffers.push_back(vertexBuffer);
    }

    void Citro3DVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer)
    {
        // indexBuffer->Bind(bufInfo);

        m_IndexBuffer = indexBuffer;
    }
}