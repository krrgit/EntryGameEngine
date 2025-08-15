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
		ET_PROFILE_FUNCTION();

		m_ViewMatrix = glm::lookAt(
			glm::vec3(0, 0, -1), // Camera position in World Space
			glm::vec3(0, 0, 1), // look direction
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

		RecalculateProjectionViewMatrix();
	}


	void PerspectiveCamera::RecalculateViewMatrix()
	{
		ET_PROFILE_FUNCTION();

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

	void PerspectiveCamera::RecalculateProjectionViewMatrix()
	{
		// Switch Projection Matrix calculation according to platform.
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::Citro3D:
				{
					// FOR 3DS PLATFORM
					// 3DS screens are sideways. See mtx_persptilt.c for more details.
					// Mtx_PerspStereoTilt()
					float fov = 80.0f;
					float invaspect = 400.0f / 240.0f;

					// Left Side
					float iod = -m_Slider3DState; // 3D effect value
					float screen = 2.0f; // No clue what this is
					float fovx = glm::radians(fov);
					float fovx_tan = tanf(fovx / 2.0f);
					float fovx_tan_invaspect = fovx_tan * invaspect;
					float nearPlane = 0.01f;
					float farPlane = 1000.0f;
					bool isLeftHanded = false;
					float shift = iod / (2.0f * screen); // 'near' not in the numerator because it cancels out in mp.r[1].z

					m_ProjectionMatrix = glm::mat3(0);

					m_ProjectionMatrix[0][1] = 1.0f / fovx_tan;
					m_ProjectionMatrix[1][0] = -1.0f / (fovx_tan * invaspect);
					m_ProjectionMatrix[1][3] = iod / 2.0f;
					m_ProjectionMatrix[2][3] = farPlane * nearPlane / (nearPlane - farPlane);
					m_ProjectionMatrix[3][2] = isLeftHanded ? 1.0f : -1.0f;
					m_ProjectionMatrix[1][2] = -m_ProjectionMatrix[3][2] * shift / fovx_tan_invaspect;
					m_ProjectionMatrix[2][2] = -m_ProjectionMatrix[3][2] * nearPlane / (nearPlane - farPlane);

					m_ProjectionMatrix = glm::transpose(m_ProjectionMatrix);
					m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;


					if (m_Slider3DState > 0.0f)
					{
						// Right Side
						iod = m_Slider3DState;
						m_ProjectionMatrixR = glm::mat3(0);

						m_ProjectionMatrixR[0][1] = 1.0f / fovx_tan;
						m_ProjectionMatrixR[1][0] = -1.0f / (fovx_tan * invaspect);
						m_ProjectionMatrixR[1][3] = iod / 2.0f;
						m_ProjectionMatrixR[2][3] = farPlane * nearPlane / (nearPlane - farPlane);
						m_ProjectionMatrixR[3][2] = isLeftHanded ? 1.0f : -1.0f;
						m_ProjectionMatrixR[1][2] = -m_ProjectionMatrixR[3][2] * shift / fovx_tan_invaspect;
						m_ProjectionMatrixR[2][2] = -m_ProjectionMatrixR[3][2] * nearPlane / (nearPlane - farPlane);

						m_ProjectionMatrixR = glm::transpose(m_ProjectionMatrixR);
						m_ViewProjectionMatrixR = m_ProjectionMatrixR * m_ViewMatrixR;
					}
				}
				break;
			default:
				{
					// For other platforms/editor (PC)
					m_ProjectionMatrix = glm::perspective(glm::radians(120.0f), 400.0f / 240.0f, 0.01f, 1000.0f);

					//m_ProjectionMatrix = glm::transpose(m_ProjectionMatrix);
					m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
				}
				break;
		}

		// Get Normals
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(m_ViewMatrix)));
		right = glm::normalize(glm::vec3(normalMatrix[0][0], normalMatrix[1][0], normalMatrix[2][0]));
		up = glm::normalize(glm::vec3(normalMatrix[0][1], normalMatrix[1][1], normalMatrix[2][1]));
		forward = glm::normalize(glm::vec3(-normalMatrix[0][2], -normalMatrix[1][2], -normalMatrix[2][2]));
	}
}
