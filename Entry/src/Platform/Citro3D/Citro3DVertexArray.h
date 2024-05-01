#pragma once

#include "Entry/Renderer/VertexArray.h"
#include <citro3d.h>

namespace Entry 
{
	class Citro3DVertexArray : public VertexArray {
    public:
        Citro3DVertexArray();
        virtual ~Citro3DVertexArray() {}

        virtual void Bind() override;
        virtual void Unbind() const override;

        virtual const void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
        virtual const void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

    private:
        std::vector<Ref<VertexBuffer>> m_VertexBuffers;
        Ref<IndexBuffer> m_IndexBuffer;

        C3D_AttrInfo m_AttrInfo;
    };
}