#pragma once

#include "Entry/Core/Core.h"

#include "VertexArray.h"
#include "Texture.h"
#include "Material.h"

#include <string>
#include <glm/glm.hpp>

namespace Entry {

	struct Mesh {
		std::string Name;
		uint32_t IndexCount;
		uint32_t IndexOffset;
		uint32_t MeshID;
		uint32_t MaterialID;
		uint32_t VertexCount;
		uint32_t PolygonCount;
	};

	class Model {
	public:
		virtual ~Model() = default;
		static Ref<Model> Create(const std::string& path = "");

		virtual Ref<VertexArray> GetVertexArray() const = 0;
		virtual const std::vector<Mesh>& GetMeshes() const = 0;
		virtual const Mesh* GetMesh(int meshID) const = 0;

		virtual void Bind() = 0;
		virtual Ref<Material> GetMaterial(int materialID) = 0;
		virtual std::vector<Ref<Material>> GetMaterials() = 0;

		virtual uint16_t GetMaterialCount() const = 0;
		virtual uint16_t GetTextuerCount() const = 0;

		virtual uint16_t GetVertexCount() const = 0;
		virtual uint16_t GetIndexCount() const = 0;
		virtual uint16_t GetPolygonCount() const = 0;

		virtual std::string GetFilePath() const = 0;
		virtual std::string GetFileName() const = 0;

	private:
	};
}