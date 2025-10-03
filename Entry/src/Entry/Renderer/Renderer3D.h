#pragma once

#include "PerspectiveCamera.h"
#include "Texture.h"
#include "Model.h"
#include "Entry/Core/Timestep.h"

#include "Entry/Renderer/Camera.h"
#include "Entry/Renderer/EditorCamera.h"
#include "Entry/Scene/Components.h"
#include "Entry/Renderer/LightEnvironment.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>


namespace Entry {
	
	class Renderer3D 
	{
		public:
			static void Init();
			static void Shutdown();

			static void BeginScene(const Camera& camera, const glm::mat4& transform, uint16_t screenSide = 0);
			static void BeginScene(const EditorCamera& camera, uint16_t screenSide = 0);
			static void EndScene();

			static void DrawModel(Ref<Model> Model, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, glm::vec4& color);
			static void DrawModel(Ref<Model> Model, const glm::mat4& transform);
			static void DrawMesh(MeshRendererComponent& mrc, const glm::mat4& transform);

			static void DrawMeshEntity(MeshRendererComponent& mrc, const glm::mat4& transform, int entityID);

			static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID = -1);
			static void DrawWireframeRect(const glm::mat4& transform, const glm::vec3& offset, const glm::vec3& size, const glm::vec4& color, int entityID = -1);
			static void DrawWireframeBox(const glm::mat4& transform, const glm::vec3& offset, const glm::vec3& size, const glm::vec4& color, int entityID = -1);

			static float GetLineWidth();
			static void SetLineWidth(float width);
	
			struct Statistics {
				uint32_t DrawCalls = 0;
				uint32_t PolygonCount = 0;
				uint32_t VertexCount = 0;
				uint32_t IndexCount = 0;
				float DeltaTime = 0;

				uint32_t GetTotalVertexCount() const { return VertexCount; }
				uint32_t GetTotalIndexCount() const { return IndexCount; }
			};
			static void ResetStats();
			static void SetStatsTimestep(Timestep ts);
			static Statistics GetStats();

			static Ref<Material> GetDefaultMaterial();
			static ShaderProgram GetDefaultShader();

			static void BindLightEnv(Ref<LightEnvironment> lightEnv);
	private:
	};

}