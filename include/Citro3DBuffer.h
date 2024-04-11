#pragma once

#include<citro3d.h>
#include "Buffer.h"

namespace Entry
{
    class Citro3DVertexBuffer : public VertexBuffer
    {
    public:
        Citro3DVertexBuffer(float *vertices, uint32_t size);
        virtual ~Citro3DVertexBuffer();

        virtual void Bind(C3D_BufInfo* bufInfo) const override;
        virtual void Unbind() const override;

        virtual const BufferLayout& GetLayout() const override { return m_Layout; }
        virtual void SetLayout(const BufferLayout layout) override { m_Layout = layout; }
    private:
        uint64_t CalculatePermutation(int attribCount);
    private:
        // uint32_t m_RendererID;
        void *m_DataPointer; 
        BufferLayout m_Layout;
    };



    class Citro3DIndexBuffer : public IndexBuffer
    {
    public:
        Citro3DIndexBuffer(uint16_t *indices, uint16_t count);
        virtual ~Citro3DIndexBuffer();

        virtual void Bind() const;
        virtual void Unbind() const;

        virtual uint16_t GetCount() const { return m_Count; }
        virtual void* GetDataPointer() const { return m_DataPointer; }
    private:
        // uint32_t m_RendererID;
        void *m_DataPointer;
        uint16_t m_Count;

    };

}
