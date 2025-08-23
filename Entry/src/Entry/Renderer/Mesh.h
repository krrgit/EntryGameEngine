#pragma once

#include "Entry/Core/Core.h"

#include "VertexArray.h"
#include "Texture.h"

#include <string>
#include <glm/glm.hpp>

namespace Entry {

	struct SubMesh {
		uint32_t indexCount;
		uint32_t indexOffset;
		uint32_t MaterialID;
	};

	struct Material {
		std::string Name;
		uint32_t TextureID;
	};

	class Mesh {
	public:
		virtual ~Mesh() = default;
		static Ref<Mesh> Create(const std::string& path);

		virtual Ref<VertexArray> GetVertexArray() const = 0;
		virtual const std::vector<SubMesh>& GetSubMeshes() const = 0;

		virtual void Bind() = 0;
		virtual void BindMaterial(uint16_t materialID) = 0;

		virtual uint16_t GetMaterialCount() const = 0;
		virtual uint16_t GetTextuerCount() const = 0;

		virtual uint16_t GetVertexCount() const = 0;
		virtual uint16_t GetIndexCount() const = 0;
		virtual uint16_t GetPolygonCount() const = 0;
	private:
	};
}