#pragma once

#include "Entry/Core/Core.h"

#include "VertexArray.h"
#include "Texture.h"
#include "Material.h"

#include <string>
#include <glm/glm.hpp>

namespace Entry {

	struct Mesh {
		uint32_t indexCount;
		uint32_t indexOffset;
		uint32_t vertexCount;
		uint32_t* vertexOffset;
		uint32_t MaterialID;
	};

	class Model {
	public:
		virtual ~Model() = default;
		static Ref<Model> Create(const std::string& path = "");

		virtual Ref<VertexArray> GetVertexArray() const = 0;
		virtual const std::vector<Mesh>& GetMeshes() const = 0;

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