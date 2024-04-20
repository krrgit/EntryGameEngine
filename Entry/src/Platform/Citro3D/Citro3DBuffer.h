#pragma once

#include<citro3d.h>
#include "Entry/Renderer/Buffer.h"

namespace Entry
{
    class Citro3DVertexBuffer : public VertexBuffer
    {
    public:
        Citro3DVertexBuffer(float* vertices, uint32_t size);
        virtual ~Citro3DVertexBuffer();


        virtual void Bind();
        virtual void Unbind() const;
    private:
        void* m_DataPointer;

        C3D_AttrInfo m_attrInfo;
        C3D_BufInfo m_bufInfo;
    };

    class Citro3DIndexBuffer : public IndexBuffer
    {
    public:
        Citro3DIndexBuffer(uint16_t* indices, uint16_t count);
        virtual ~Citro3DIndexBuffer();


        virtual void Bind();
        virtual void Unbind() const;

        virtual uint16_t GetCount() const { return m_Count; }
        virtual void* GetDataPointer() const { return m_DataPointer; }
    private:
        void* m_DataPointer;
        uint16_t m_Count;

    };

}
