#pragma once

#include "Entry/Renderer/Mesh.h"

#include "Entry/Renderer/VertexArray.h"
#include "Entry/Renderer/Buffer.h"


namespace Entry {
	class Citro3DMesh: public Mesh
	{
	public:
		Citro3DMesh(const std::string& path);
		virtual ~Citro3DMesh();

		virtual Ref<VertexArray> GetVertexArray() const override { return m_VertexArray; }

		virtual void Bind() override;

		virtual uint16_t GetVertexCount() const { return m_VertexCount; }
		virtual uint16_t GetIndexCount() const { return m_IndexCount; }

	private:
		std::string m_Name;
		Ref<VertexArray> m_VertexArray;
		uint16_t m_VertexCount, m_IndexCount;

	};

}