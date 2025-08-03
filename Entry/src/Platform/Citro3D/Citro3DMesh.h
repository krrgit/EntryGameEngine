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

	private:
		std::string m_Name;
		Ref<VertexArray> m_VertexArray;
	};

}