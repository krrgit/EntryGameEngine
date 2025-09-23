#include "etpch.h"
#include "OpenGLModel.h"
#include "Entry/Utils/ModelUtils.h"
#include "Entry/Renderer/Renderer3D.h"

#include <sstream>
#include <initializer_list>

// assimp include files. These three are usually needed.
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

namespace Entry {

	static void AssimpToBuffers(
		std::vector<float>* outVertices,
		std::vector<uint16_t>* outIndices,
		const aiScene* g_scene)
	{
		outVertices->clear();
		outIndices->clear();

		uint16_t runningIndex = 0;

		for (unsigned int m = 0; m < g_scene->mNumMeshes; ++m)
		{
			const aiMesh* mesh = g_scene->mMeshes[m];

			for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
			{
				const aiFace& face = mesh->mFaces[f];
				// Assuming triangulated mesh
				for (unsigned int i = 0; i < face.mNumIndices; ++i)
				{
					unsigned int vertIndex = face.mIndices[i];

					const aiVector3D& pos = mesh->mVertices[vertIndex];

					float tu = 0, tv = 0;
					if (mesh->HasTextureCoords(0))
					{
						tu = mesh->mTextureCoords[0][vertIndex].x;
						tv = mesh->mTextureCoords[0][vertIndex].y;
					}

					float nx = 0, ny = 0, nz = 0;
					if (mesh->HasNormals())
					{
						nx = mesh->mNormals[vertIndex].x;
						ny = mesh->mNormals[vertIndex].y;
						nz = mesh->mNormals[vertIndex].z;
					}

					// Write vertex
					outVertices->push_back(-pos.x);
					outVertices->push_back(pos.y);
					outVertices->push_back(-pos.z);

					outVertices->push_back(tu);
					outVertices->push_back(tv);

					outVertices->push_back(-nx);
					outVertices->push_back(ny);
					outVertices->push_back(-nz);

					// Index just refers to the vertex we *just* pushed
					outIndices->push_back(runningIndex++);
				}
			}
		}
	}


	static void AssimpCreateMeshes(
		std::vector<Mesh>& meshes,
		std::vector<Ref<Material>>& materials,
		std::vector<Ref<Texture2D>>& textures,
		const aiScene* g_scene,
		std::string& directory)
	{
		meshes.clear();
		materials.clear();
		textures.clear();

		// Get Materials
		for (uint32_t i = 0; i < g_scene->mNumMaterials; ++i)
		{
			aiMaterial* mat = g_scene->mMaterials[i];
			int texCount = mat->GetTextureCount(aiTextureType_DIFFUSE);
			Ref<Texture2D> diffuseMap;

			// Add Texture
			if (texCount > 0)
			{
				aiString textureName;
				mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textureName);
				diffuseMap = Texture2D::Create(directory + "\\" + textureName.C_Str());
				textures.push_back(diffuseMap);
			}

			aiColor3D ambient, diffuse, specular, emissive;
			mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			mat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);

			MaterialProps props = {
				{	// Material Values
					ambient.r,  ambient.g,  ambient.b,
					diffuse.r,  diffuse.g,  diffuse.b,
					specular.r, specular.g, specular.b,
					specular.r, specular.g, specular.b,
					emissive.r, emissive.g, emissive.b,
				},
				g_scene->mMaterials[i]->GetName().C_Str(),
				texCount > 0 ? diffuseMap : nullptr,
				Renderer3D::GetDefaultShader()
			};

			materials.push_back(Material::Create(props));
		}


		// Get Meshes
		uint16_t indexOffset = 0;
		for (int i = 0; i < g_scene->mNumMeshes; ++i)
		{
			auto& aimesh = g_scene->mMeshes[i];

			Mesh mesh{
				aimesh->mName.C_Str(),	// Name
				aimesh->mNumFaces * 3,	// Index Count
				indexOffset,			// Index Offset
				i,						// MeshID
				aimesh->mMaterialIndex, // MaterialID
				aimesh->mNumFaces * 3,	// Vertex Count
				aimesh->mNumFaces		// Polygon Count
			};
			meshes.push_back(mesh);

			indexOffset += static_cast<uint16_t>(aimesh->mNumFaces * 3);
		}

		meshes.shrink_to_fit();
		materials.shrink_to_fit();
		textures.shrink_to_fit();
	}

	OpenGLModel::OpenGLModel(const std::string& path)
	{
		m_FileName = ExtractFileName(path);
		m_FilePath = path;
        std::string directory = ExtractDirectory(path);

		std::vector<float> vertices;
		std::vector<uint16_t> indices;

		Assimp::Importer importer;
		const aiScene* g_scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Quality);

		// If the import failed, report it
		if (g_scene == nullptr)
			ET_CORE_ERROR("Assimp: Failed to load {0}", path);

		AssimpToBuffers(&vertices, &indices, g_scene);
		AssimpCreateMeshes(m_Meshes, m_Materials, m_Textures, g_scene, directory);

		//Ref<fastObjMesh> obj_Model = Ref<fastObjMesh>(fast_obj_read(path.c_str()));
		//FastOBJToBuffers(&vertices, &indices, obj_Model);
		//FastOBJCreateMeshes(m_Meshes, m_Materials, m_Textures, obj_Model);

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

		if (m_Materials.size() == 0)
			m_Materials.push_back(Renderer3D::GetDefaultMaterial());

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
			ET_CORE_TRACE("Meshes: {0}", m_Meshes.size());
			ET_CORE_TRACE("Materials: {0}", m_MaterialCount);
			ET_CORE_TRACE("Textures: {0}", m_TextureCount);
		}
	}

	OpenGLModel::~OpenGLModel()
	{
		ET_CORE_ERROR("Destroyed {0}", m_FileName.c_str());
	}

	void OpenGLModel::Bind()
	{
		m_VertexArray->Bind();
	}
}