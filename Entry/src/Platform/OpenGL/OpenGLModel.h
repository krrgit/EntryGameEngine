#pragma once

#include "Entry/Renderer/Model.h"

#include "Entry/Renderer/VertexArray.h"
#include "Entry/Renderer/Buffer.h"


namespace Entry {
	class OpenGLModel: public Model
	{
	public:
		OpenGLModel(const std::string& path);
		virtual ~OpenGLModel();

		virtual Ref<VertexArray> GetVertexArray() const override { return m_VertexArray; }
		virtual const std::vector<Mesh>& GetMeshes() const override { return m_Meshes; }

		virtual void Bind() override;

		virtual Ref<Material> GetMaterial(int materialID) override { return materialID < m_MaterialCount ? m_Materials[materialID] : nullptr; };
		virtual std::vector<Ref<Material>> GetMaterials() override { return m_Materials; };

		virtual uint16_t GetMaterialCount() const { return m_MaterialCount; }
		virtual uint16_t GetTextuerCount() const { return m_TextureCount; }

		virtual uint16_t GetPolygonCount() const { return m_PolygonCount; }
		virtual uint16_t GetVertexCount() const { return m_VertexCount; }
		virtual uint16_t GetIndexCount() const { return m_IndexCount; }

		virtual std::string GetFileName() const { return m_FileName; }
		virtual std::string GetFilePath() const { return m_FilePath; }

	private:
		std::string m_FileName = "";
		std::string m_FilePath = "";
		Ref<VertexArray> m_VertexArray;
		std::vector<Mesh> m_Meshes;
		std::vector<Ref<Material>> m_Materials;
		std::vector<Ref<Texture2D>> m_Textures;
		uint16_t m_MaterialCount, m_TextureCount = 0;
		uint16_t m_PolygonCount, m_VertexCount, m_IndexCount = 0;
	};

}