#include "etpch.h"
#include "Citro3DMesh.h"

#include <fast_obj.h>

#include <sstream>
#include <initializer_list>

namespace Entry {

	static void FastOBJToBuffers(std::vector<float>* outVertices, std::vector<uint16_t>* outIndices, Ref<fastObjMesh> obj_mesh)
	{
		ET_PROFILE_FUNCTION();

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
		ET_PROFILE_FUNCTION();

		submeshes.clear();
		materials.clear();
		textures.clear();

		// Get Materials
		for (uint32_t i = 0; i < obj_mesh->material_count; ++i)
		{
			Material mat = { obj_mesh->materials[i].name, obj_mesh->materials[i].map_Kd - 1 };
			materials.push_back(mat);
		}

		//// Get Textures
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

			SubMesh submesh{
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
	
	Citro3DMesh::Citro3DMesh(const std::string& path)
	{
		ET_PROFILE_FUNCTION();

		std::string romfsPath = "romfs:/" + path;
		
		m_Name = path;
		// TODO: Update to full support OBJ files
		std::vector<float> vertices;
		std::vector<uint16_t> indices;

		Ref<fastObjMesh> obj_mesh = Ref<fastObjMesh>(fast_obj_read(romfsPath.c_str()));

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
			printf("Failed to load \"%s\"\n", m_Name.c_str());
		}
		else {
			printf("Loaded \"%s\" successfully! Indices: %d\n", m_Name.c_str(), indices.size());
			printf("SubMeshes: %d\n", m_SubMeshes.size());
			printf("Materials: %d\n", m_MaterialCount);
			printf("Textures: %d\n", m_TextureCount);
		}
	}
	Citro3DMesh::~Citro3DMesh()
	{
	}

	void Citro3DMesh::Bind()
	{
		m_VertexArray->Bind();
	}
	void Citro3DMesh::BindMaterial(uint16_t materialID)
	{
		ET_PROFILE_FUNCTION();
		if (m_MaterialCount == 0) return;
		m_Textures[m_Materials[materialID].TextureID]->Bind(0);
	}
}