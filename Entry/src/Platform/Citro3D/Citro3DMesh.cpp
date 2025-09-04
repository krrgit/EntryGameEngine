#include "etpch.h"
#include "Citro3DMesh.h"

#include <sstream>
#include <initializer_list>

#include "Entry/Utils/ModelUtils.h"

namespace Entry {	
	Citro3DMesh::Citro3DMesh(const std::string& path)
	{
		ET_PROFILE_FUNCTION();

		m_FileName = ExtractFileName(path);
		m_FilePath = "romfs:/" + path;
		
		std::vector<float> vertices;
		std::vector<uint16_t> indices;

		Ref<fastObjMesh> obj_mesh = Ref<fastObjMesh>(fast_obj_read(m_FilePath.c_str()));

		FastOBJToBuffers(&vertices, &indices, obj_mesh);
		CreateSubMeshes(m_SubMeshes, m_Materials, m_Textures, obj_mesh);

		m_VertexArray = VertexArray::Create();

		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(float)));

		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal" }
			});

		m_VertexArray->AddVertexBuffer(vertexBuffer);

		Ref<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices.data(), indices.size()));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_VertexCount = vertices.size() / 8;
		m_IndexCount = indices.size();
		m_PolygonCount = m_IndexCount / 3;

		m_MaterialCount = m_Materials.size();
		m_TextureCount = m_Textures.size();

		if (indices.size() == 0) {
			ET_CORE_INFO("Failed to load \"{0}\"!", m_FileName.c_str());
		}
		else {
			ET_CORE_INFO("Loaded \"{0}\" successfully!", m_FileName.c_str());
			ET_CORE_TRACE("Indices: {0}", m_IndexCount);
			ET_CORE_TRACE("Vertices: {0}", m_VertexCount);
			ET_CORE_TRACE("SubMeshes: {0}", m_SubMeshes.size());
			ET_CORE_TRACE("Materials: {0}", m_MaterialCount);
			ET_CORE_TRACE("Textures: {0}", m_TextureCount);
		}
	}
	Citro3DMesh::~Citro3DMesh()
	{
	}

	void Citro3DMesh::Bind()
	{
		m_VertexArray->Bind();
	}
}