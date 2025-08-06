#include "etpch.h"
#include <citro3d.h>
#include "Citro3DBuffer.h"

namespace Entry
{
	////////////////////////////////////////////////////////////////////////
	// VertexBuffer ////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	Citro3DVertexBuffer::Citro3DVertexBuffer(float* vertices, uint32_t size) {
		ET_PROFILE_FUNCTION();

		m_DataPointer = linearAlloc(size);
		memcpy(m_DataPointer, vertices, size);

		BufInfo_Init(&m_bufInfo);
	}

	Citro3DVertexBuffer::~Citro3DVertexBuffer() {
		ET_PROFILE_FUNCTION();

		linearFree(m_DataPointer);
	}

	void Citro3DVertexBuffer::Bind() {
		ET_PROFILE_FUNCTION();
		
		C3D_SetBufInfo(&m_bufInfo);
	}

	void Citro3DVertexBuffer::Unbind() const {
		ET_PROFILE_FUNCTION();

		C3D_SetBufInfo(0);
	}

	void Citro3DVertexBuffer::SetLayout(const BufferLayout layout) 
	{
		ET_PROFILE_FUNCTION();

		m_Layout = layout;
		BufInfo_Add(&m_bufInfo, m_DataPointer, m_Layout.GetStride(), m_Layout.GetAttribCount(), m_Layout.GetPermutation());
	}

	////////////////////////////////////////////////////////////////////////
	// IndexBuffer /////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	Citro3DIndexBuffer::Citro3DIndexBuffer(uint16_t* indices, uint16_t count) 
		: m_Count(count)
	{
		ET_PROFILE_FUNCTION();

		m_DataPointer = linearAlloc(count * sizeof(uint16_t));
		memcpy(m_DataPointer, indices, count * sizeof(uint16_t));
	}

	Citro3DIndexBuffer::~Citro3DIndexBuffer() {
		ET_PROFILE_FUNCTION();

		linearFree(m_DataPointer);
	}

	void Citro3DIndexBuffer::Bind() {
		ET_PROFILE_FUNCTION();
		
		//C3D_SetBufInfo(&m_bufInfo);
	}

	void Citro3DIndexBuffer::Unbind() const {
		ET_PROFILE_FUNCTION();

		//C3D_SetBufInfo(0);
	}
}