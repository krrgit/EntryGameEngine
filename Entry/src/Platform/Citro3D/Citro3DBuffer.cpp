#include "etpch.h"
#include <citro3d.h>
#include "Citro3DBuffer.h"

namespace Entry
{
    ////////////////////////////////////////////////////////////////////////
    // VertexBuffer ////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////

    Citro3DVertexBuffer::Citro3DVertexBuffer(float *vertices, uint32_t size)
    {
        // Size is for all vertices, stride is per vertex
        // Allocate on VRAM
        m_DataPointer = linearAlloc(size);
	    memcpy(m_DataPointer, vertices, size);

        BufInfo_Init(&m_bufInfo);
        BufInfo_Add(&m_bufInfo, m_DataPointer, m_Layout.GetStride(), m_Layout.GetAttribCount(), m_Layout.GetPermutation());
    }

    Citro3DVertexBuffer::~Citro3DVertexBuffer()
    {
        // Free the buffer
        linearFree(m_DataPointer);
    }

    void Citro3DVertexBuffer::Bind()
    {
        C3D_SetBufInfo(&m_bufInfo);
        //ET_CORE_INFO("s: {0} | ac: {1}| perm: {2}\n", m_Layout.GetStride(), m_Layout.GetAttribCount(), m_Layout.GetPermutation());
    }
    void Citro3DVertexBuffer::Unbind() const
    {
        C3D_SetBufInfo(0);
    }

    ////////////////////////////////////////////////////////////////////////
    // IndexBuffer /////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////

    Citro3DIndexBuffer::Citro3DIndexBuffer(uint16_t *indices, uint16_t count) 
    : m_Count(count)
    {
        m_DataPointer = linearAlloc(count * sizeof(uint16_t));
	    memcpy(m_DataPointer, indices, count * sizeof(uint16_t));

        // BufInfo_Add(C3D_GetBufInfo(), m_DataPointer, sizeof(uint16_t), 1, 0x1);
    }

    Citro3DIndexBuffer::~Citro3DIndexBuffer()
    {
        // Free the buffer
        linearFree(m_DataPointer);
    }

    void Citro3DIndexBuffer::Bind() const
    {
        // might be redundant here
    }
    void Citro3DIndexBuffer::Unbind() const
    {
        // not implemented in Citro3D
    }
}