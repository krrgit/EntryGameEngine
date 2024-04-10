#include <3ds.h>
#include <citro3d.h>
#include "Citro3DBuffer.h"

namespace Entry
{
    ////////////////////////////////////////////////////////////////////////
    // VertexBuffer ////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////

    Citro3DVertexBuffer::Citro3DVertexBuffer(float *vertices, uint32_t size)
    {
        m_DataPointer = linearAlloc(size);
	    memcpy(m_DataPointer, vertices, size);

        // TODO: pass # of attributes; figure out last argument
        BufInfo_Add(C3D_GetBufInfo(), m_DataPointer, size, 1, 0x0);
    }

    Citro3DVertexBuffer::~Citro3DVertexBuffer()
    {
        // Free the buffer
        linearFree(m_DataPointer);
    }

    void Citro3DVertexBuffer::Bind() const
    {
        // might be redundant here
    }
    void Citro3DVertexBuffer::Unbind() const
    {
        // not implemented in Citro3D
    }

    ////////////////////////////////////////////////////////////////////////
    // IndexBuffer /////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////

    Citro3DIndexBuffer::Citro3DIndexBuffer(uint16_t *indices, uint16_t count) 
    : m_Count(count)
    {
        m_DataPointer = linearAlloc(count * sizeof(uint16_t));
	    memcpy(m_DataPointer, indices, count * sizeof(uint16_t));

        // TODO: pass # of attributes; figure out last argument
        BufInfo_Add(C3D_GetBufInfo(), m_DataPointer, count * sizeof(uint16_t), 1, 0x10);

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