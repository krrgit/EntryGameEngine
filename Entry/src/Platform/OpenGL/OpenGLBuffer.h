#pragma once

#include "Entry/Renderer/Buffer.h"

namespace Entry {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() override;
		virtual void Unbind() const override;
		
		virtual void SetData(const void* data, uint32_t size) override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout layout) override { m_Layout = layout; }
	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint16_t* indices, uint16_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind();
		virtual void Unbind() const;

		virtual uint16_t GetCount() const { return m_Count; }
		virtual void* GetDataPointer() const { return (void*)m_RendererID;  }
	private:
		uint32_t m_RendererID;
		uint16_t m_Count;
	};

}