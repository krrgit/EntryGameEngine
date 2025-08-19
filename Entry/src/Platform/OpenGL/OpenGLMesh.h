#pragma once

#include "Entry/Renderer/Mesh.h"

#include "Entry/Renderer/VertexArray.h"
#include "Entry/Renderer/Buffer.h"


namespace Entry {
	class OpenGLMesh: public Mesh
	{
	public:
		OpenGLMesh(const std::string& path);
		virtual ~OpenGLMesh();

		virtual Ref<VertexArray> GetVertexArray() const override { return m_VertexArray; }

		virtual void Bind() override;

	private:
		std::string m_Name;
		Ref<VertexArray> m_VertexArray;
	};

}