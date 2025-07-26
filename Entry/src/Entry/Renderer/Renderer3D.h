#pragma once

#include "PerspectiveCamera.h"
#include "Texture.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Entry {
	
	class Renderer3D 
	{
		public:
			static void Init();
			static void Shutdown();

			static void BeginScene(const PerspectiveCamera& camera);
			static void EndScene();

			// Primitives
			static void DrawQuad(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color);
			static void DrawCube(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color);
			static void DrawQuad(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const Ref<Texture2D>& texture);
			static void DrawCube(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const Ref<Texture2D>& texture);

	};

}