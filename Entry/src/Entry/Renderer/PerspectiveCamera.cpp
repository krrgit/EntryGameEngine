#include "etpch.h"
#include "PerspectiveCamera.h"

#include "glm/gtc/matrix_transform.hpp"

#include "Renderer.h"

namespace Entry {

	PerspectiveCamera::PerspectiveCamera(float _left, float _right, float _bottom, float _top)
	{
		SetProjection(_left, _right, _bottom, _top);
	}


	void PerspectiveCamera::SetProjection(float _left, float _right, float _bottom, float _top)
	{	
		m_ViewMatrix = glm::lookAt(
			glm::vec3(0, 0, -1), // Camera position in World Space
			glm::vec3(0, 0, 1), // look direction
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

		// Switch Projection Matrix calculation according to platform.
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:    
				{
					// For other platforms/editor (PC)
					m_ProjectionMatrix = glm::perspective(glm::radians(80.0f), 400.0f / 240.0f, 0.01f, 1000.0f);
				}
			case RendererAPI::API::Citro3D: 
				{
					// FOR 3DS PLATFORM
					// 3DS screens are sideways. See mtx_persptilt.c for more details.
					float fov = 80.0f;
			
					float fovx = glm::radians(fov);
					float fovx_tan = tanf(fovx/2.0f);
					float invaspect = 400.0f/240.0f;
					float near = 0.01f;
					float far  = 1000.0f;
					bool isLeftHanded = false;

					m_ProjectionMatrix = glm::mat3(0);
	
					m_ProjectionMatrix[0][1] = 1.0f / fovx_tan;
					m_ProjectionMatrix[1][0] = -1.0f / (fovx_tan*invaspect);
					m_ProjectionMatrix[2][3] = far*near / (near - far);
					m_ProjectionMatrix[3][2] = isLeftHanded ? 1.0f : -1.0f;
					m_ProjectionMatrix[2][2] = -m_ProjectionMatrix[2][3] * near / (near - far);
				};
		}

		
		m_ProjectionMatrix = glm::transpose(m_ProjectionMatrix);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

		// Get Normals
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(m_ViewMatrix)));
		right = glm::normalize(glm::vec3(normalMatrix[0][0], normalMatrix[1][0], normalMatrix[2][0]));
		up = glm::normalize(glm::vec3(normalMatrix[0][1], normalMatrix[1][1], normalMatrix[2][1]));
		forward = glm::normalize(glm::vec3(-normalMatrix[0][2], -normalMatrix[1][2], -normalMatrix[2][2]));
	}


	void PerspectiveCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform =	glm::translate(glm::mat4(1.0f), m_Position) *
								glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0,0,1)) *
								glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0,1,0)) *
								glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1,0,0));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

		// Get Normals
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(m_ViewMatrix)));
		right = glm::normalize(glm::vec3(normalMatrix[0][0], normalMatrix[1][0], normalMatrix[2][0]));
		up = glm::normalize(glm::vec3(normalMatrix[0][1], normalMatrix[1][1], normalMatrix[2][1]));
		forward = glm::normalize(glm::vec3(-normalMatrix[0][2], -normalMatrix[1][2], -normalMatrix[2][2]));

		/*consoleClear();

		glm::mat4 glm_in = normalMatrix;
		printf("%.1f %.1f %.1f\n%.1f %.1f %.1f\n%.1f %.1f %.1f\n",
			glm_in[0].x, glm_in[0].y, glm_in[0].z,
			glm_in[1].x, glm_in[1].y, glm_in[1].z,
			glm_in[2].x, glm_in[2].y, glm_in[2].z
		);*/
	}
}