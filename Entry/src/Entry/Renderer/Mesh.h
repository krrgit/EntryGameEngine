#pragma once

#include "Entry/Core/Core.h"

#include "VertexArray.h"

#include <string>
#include <glm/glm.hpp>

namespace Entry {
	class Mesh {
	public:
		virtual ~Mesh() = default;
		static Ref<Mesh> Create(const std::string& path);

		virtual Ref<VertexArray> GetVertexArray() const = 0;

		virtual void Bind() = 0;

		virtual uint16_t GetVertexCount() const = 0;
		virtual uint16_t GetIndexCount() const = 0;
		virtual uint16_t GetPolygonCount() const = 0;
	private:
	};
}