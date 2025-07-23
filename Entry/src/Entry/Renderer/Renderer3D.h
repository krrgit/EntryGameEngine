#pragma once

#include "PerspectiveCamera.h"

#include <glm/gtc/type_ptr.hpp>

namespace Entry {
	
	class Renderer3D 
	{
		public:
			static void Init();
			static void Shutdown();

			static void BeginScene(const PerspectiveCamera& camera);
			static void EndScene();

			// Primitives
			static void DrawQuad(const C3D_Mtx& transform, glm::vec4& color);
			static void DrawCube(const C3D_Mtx& transform, glm::vec4& color);

	};

}