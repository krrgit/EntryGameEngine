#include "etpch.h"
#include <citro3d.h>
#include "Citro3DBuffer.h"

namespace Entry
{
	////////////////////////////////////////////////////////////////////////
	// VertexBuffer ////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	Citro3DVertexBuffer::Citro3DVertexBuffer(float* vertices, uint32_t size) {
		m_DataPointer = linearAlloc(size);
		memcpy(m_DataPointer, vertices, size);

		BufInfo_Init(&m_bufInfo);
	}

	Citro3DVertexBuffer::~Citro3DVertexBuffer() {
		linearFree(m_DataPointer);
	}

	void Citro3DVertexBuffer::Bind() {
		C3D_SetBufInfo(&m_bufInfo);
	}

	void Citro3DVertexBuffer::Unbind() const {
		C3D_SetBufInfo(0);
	}

	void Citro3DVertexBuffer::SetLayout(const BufferLayout layout) 
	{
		m_Layout = layout;
		BufInfo_Add(&m_bufInfo, m_DataPointer, m_Layout.GetStride(), m_Layout.GetAttribCount(), m_Layout.GetPermutation());
	}

	////////////////////////////////////////////////////////////////////////
	// IndexBuffer /////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	Citro3DIndexBuffer::Citro3DIndexBuffer(uint16_t* indices, uint16_t count) 
		: m_Count(count)
	{
		m_DataPointer = linearAlloc(count * sizeof(uint16_t));
		memcpy(m_DataPointer, indices, count * sizeof(uint16_t));
	}

	Citro3DIndexBuffer::~Citro3DIndexBuffer() {
		linearFree(m_DataPointer);
	}

	void Citro3DIndexBuffer::Bind() {
		//C3D_SetBufInfo(&m_bufInfo);
	}

	void Citro3DIndexBuffer::Unbind() const {
		//C3D_SetBufInfo(0);
	}
}