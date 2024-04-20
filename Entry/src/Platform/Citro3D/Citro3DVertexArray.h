#pragma once

#include "VertexArray.h"
#include <citro3d.h>

namespace Entry 
{
    class Citro3DVertexArray : public VertexArray
    {
    public:
        Citro3DVertexArray();
        virtual ~Citro3DVertexArray() {}

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
        virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

        virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers;}
        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
    private:
        std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
        C3D_AttrInfo m_attrInfo;
    };
}