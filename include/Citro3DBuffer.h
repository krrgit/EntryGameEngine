#pragma once

#include "Buffer.h"

namespace Entry
{
    class Citro3DVertexBuffer : public VertexBuffer
    {
    public:
        Citro3DVertexBuffer(float *vertices, uint32_t size);
        virtual ~Citro3DVertexBuffer();

        virtual void Bind() const;
        virtual void Unbind() const;
    private:
        // uint32_t m_RendererID;
        void *m_DataPointer; 
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
