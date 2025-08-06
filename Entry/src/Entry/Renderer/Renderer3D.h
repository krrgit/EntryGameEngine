#pragma once

#include "PerspectiveCamera.h"
#include "Texture.h"
#include "Mesh.h"
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
			static void Flush();

			// Primitives
			static void DrawQuad(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color);
			static void DrawCube(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color);
			static void DrawQuad(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
			static void DrawCube(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
			static void DrawCube(const glm::vec3& position, glm::vec4& color);

			static void DrawMesh(Ref<Mesh> mesh, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color);
			static void DrawMesh(Ref<Mesh> mesh, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
	
			struct Statistics {
				uint32_t DrawCalls = 0;
				uint32_t PolygonCount = 0;
				uint32_t VertexCount = 0;
				uint32_t IndexCount = 0;

				uint32_t GetTotalVertexCount() const { return VertexCount; }
				uint32_t GetTotalIndexCount() const { return IndexCount; }
			};
			static void ResetStats();
			static Statistics GetStats();
	private:
		static int GetBatch(Ref<Texture2D> textureRef, uint32_t indexCount);
		//static void FlushAndReset();
	};

}