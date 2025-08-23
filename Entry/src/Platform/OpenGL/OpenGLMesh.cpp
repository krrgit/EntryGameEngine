#include "etpch.h"
#include "OpenGLMesh.h"

#include <fast_obj.h>

#include <sstream>
#include <initializer_list>

namespace Entry {

	static void FastOBJToBuffers(std::vector<float>* outVertices, std::vector<uint16_t>* outIndices, Ref<fastObjMesh> obj_mesh)
	{
		outVertices->clear();
		outIndices->clear();

		uint32_t indexOffset = 0;

		for (uint32_t i = 0; i < obj_mesh->face_count; ++i) {
			uint32_t faceVertices = obj_mesh->face_vertices[i];

			for (uint32_t j = 0; j < faceVertices; ++j) {
				fastObjIndex idx = obj_mesh->indices[indexOffset + j];


				float px = obj_mesh->positions[3 * idx.p + 0];
				float py = obj_mesh->positions[3 * idx.p + 1];
				float pz = obj_mesh->positions[3 * idx.p + 2];

				float u = 0.0f, v = 0.0f;
				if (idx.t != 0 && obj_mesh->texcoords) {
					u = obj_mesh->texcoords[2 * idx.t + 0];
					v = 1.0f - obj_mesh->texcoords[2 * idx.t + 1]; // Flip V
				}

				float nx = 0.0f, ny = 0.0f, nz = 0.0f;
				if (idx.n != 0 && obj_mesh->normals) {
					nx = obj_mesh->normals[3 * idx.n + 0];
					ny = obj_mesh->normals[3 * idx.n + 1];
					nz = obj_mesh->normals[3 * idx.n + 2];
				}

				// Append to vertex buffer
				outVertices->push_back(px);
				outVertices->push_back(py);
				outVertices->push_back(pz);
				outVertices->push_back(u);
				outVertices->push_back(v);
				outVertices->push_back(nx);
				outVertices->push_back(ny);
				outVertices->push_back(nz);

				// Index is just the order of the vertices
				outIndices->push_back(static_cast<uint16_t>((outVertices->size() / 8) - 1));
			}
				indexOffset += faceVertices;
		}
	}

	static void CreateSubMeshes(std::vector<SubMesh>& submeshes, std::vector<Material>& materials, std::vector<Ref<Texture2D>>& textures, Ref<fastObjMesh> obj_mesh)
	{
		submeshes.clear();
		materials.clear();
		textures.clear();

		// Get Materials
		for (uint32_t i = 0; i < obj_mesh->material_count; ++i)
		{
			Material mat = { obj_mesh->materials[i].name, obj_mesh->materials[i].map_Kd-1};
			materials.push_back(mat);
		}

		// Get Textures
		for (uint32_t i = 0; i < obj_mesh->texture_count; ++i)
		{
			if (obj_mesh->textures[i].path == nullptr) continue;
			Ref<Texture2D> tex = Texture2D::Create(obj_mesh->textures[i].path);
			textures.push_back(tex);
		}

		// Create submeshes from objects (-o)
		for (uint32_t o = 0; o < obj_mesh->object_count; ++o) 
		{
			auto currentObj = obj_mesh->objects[o];
			uint32_t nextIndexOffset = o + 1 >= obj_mesh->object_count ? obj_mesh->index_count : obj_mesh->objects[o + 1].index_offset;
			uint32_t indexCount = nextIndexOffset - currentObj.index_offset;

			SubMesh submesh {
				indexCount,
				currentObj.index_offset, 
				obj_mesh->face_materials[currentObj.face_offset] 
			};
			submeshes.push_back(submesh);
		}

		submeshes.resize(submeshes.size());
		materials.resize(materials.size());
		textures.resize(textures.size());
	}

	OpenGLMesh::OpenGLMesh(const std::string& path)
	{
		m_Name = path;
		std::vector<float> vertices;
		std::vector<uint16_t> indices;

		Ref<fastObjMesh> obj_mesh = Ref<fastObjMesh>(fast_obj_read(path.c_str()));

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

		printf("Loaded \"%s\" successfully!\n", m_Name.c_str());
		printf("\"%s\" indices: %d\n", m_Name.c_str(), indices.size());
	}

	OpenGLMesh::~OpenGLMesh()
	{
	}

	void OpenGLMesh::Bind()
	{
		m_VertexArray->Bind();
	}
	void OpenGLMesh::BindMaterial(uint16_t materialID)
	{
		if (m_MaterialCount == 0) return;
		m_Textures[m_Materials[materialID].TextureID]->Bind(0);
	}
}