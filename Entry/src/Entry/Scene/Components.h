#pragma once

#include "Entry/Core/Core.h"
#include "Entry/Renderer/Mesh.h"

#include <glm/glm.hpp>

namespace Entry {

	struct TransformComponent {
		glm::mat4 Transform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)
			: Transform(transform) {}

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }
	};

	struct MeshRendererComponent {
		Ref<Mesh> mesh;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
		MeshRendererComponent(const Ref<Mesh> _mesh)
			: mesh(_mesh) {}
	};



}