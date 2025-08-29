#include "etpch.h"
#include "PerspectiveCamera.h"

#include "glm/gtc/matrix_transform.hpp"

#include "Renderer.h"

namespace Entry {



	PerspectiveCamera::PerspectiveCamera(float _aspectRatio, float _fov)
	{
		m_ViewMatrix = glm::lookAt(
			glm::vec3(0, 0, 0), // Camera position in World Space
			glm::vec3(0, 0, 1), // look direction
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
		SetProjection(_aspectRatio, _fov);
	}


	void PerspectiveCamera::SetProjection(float _aspectRatio, float _fov)
	{	
		ET_PROFILE_FUNCTION();

		m_AspectRatio = _aspectRatio;
		m_FOV = _fov;

		RecalculateProjectionViewMatrix();
	}

	void PerspectiveCamera::SetAspectRatio(float _aspectRatio)
	{
		m_AspectRatio = _aspectRatio;

		RecalculateProjectionViewMatrix();
	}

	void PerspectiveCamera::SetFOV(float _fov)
	{
		m_FOV = _fov;

		RecalculateProjectionViewMatrix();
	}


	void PerspectiveCamera::CalculateProjection(glm::mat4& out, float aspectRatio, float fov, float iod, bool leftSide)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::Citro3D:
		{

			// FOR 3DS PLATFORM
			// 3DS screens are sideways. See mtx_persptilt.c for more details.
			// Mtx_PerspStereoTilt()
			float invaspect = aspectRatio; // Inverse relative to 3DS screen ()

			iod = leftSide ? -iod : iod; // shift for stereo 3D
			float screen = 2.0f; // No clue what this is
			float fovx = glm::radians(fov);
			float fovx_tan = tanf(fovx / 2.0f);
			float fovx_tan_invaspect = fovx_tan * invaspect;
			float nearPlane = 0.01f;
			float farPlane = 1000.0f;
			bool isLeftHanded = false;
			float eyeShift = iod / (2.0f * screen); // 'near' not in the numerator because it cancels out in mp.r[1].z

			out = glm::mat3(0);

			out[0][1] = 1.0f / fovx_tan;
			out[1][0] = -1.0f / (fovx_tan * invaspect);
			out[1][3] = iod / 2.0f;
			out[2][3] = farPlane * nearPlane / (nearPlane - farPlane);
			out[3][2] = isLeftHanded ? 1.0f : -1.0f;
			out[1][2] = -out[3][2] * eyeShift / fovx_tan_invaspect;
			out[2][2] = -out[3][2] * nearPlane / (nearPlane - farPlane);

			out = glm::transpose(out);
		}
			break;
		default:
		{
			// For OTHER PLATFORMS/EDITOR (PC)
			// Left Side
			iod = leftSide ? -iod : iod; // 3D effect value
			float screen = 2.0f; // No clue what this is
			float nearPlane = 0.01f;
			float farPlane = 1000.0f;
			bool isLeftHanded = false;

			// Stereo shift (sign flips per eye)
			float eyeShift = iod / (2.0f * screen); // 'near' not in the numerator because it cancels out in mp.r[1].z

			float fovY = glm::radians(fov);
			float tanHalfFovY = tanf(fovY / 2.0f);
			float tanHalfFovX = tanHalfFovY * aspectRatio;

			out = glm::mat3(0);

			// Column 0 (X axis scaling)
			out[0][0] = 1.0f / tanHalfFovX;

			// Column 1 (Y axis scaling — 3DS screen tilt handled here)
			out[1][1] = 1.0f / tanHalfFovY;
			out[1][2] = -((isLeftHanded ? 1.0f : -1.0f) * eyeShift) / tanHalfFovX; // tilt offset

			// Column 2 (Z)
			out[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
			out[2][3] = -1.0f;

			// Column 3 (Translation / depth)
			out[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
		}
			break;
		}
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

		RecalculateNormals();
	}

	void PerspectiveCamera::RecalculateProjectionViewMatrix()
	{
		CalculateProjection(m_ProjectionMatrix, m_AspectRatio, m_FOV, m_Slider3DState);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

		// 3DS Only
		if (m_Slider3DState > 0.0f)
		{
			// Right Side
			CalculateProjection(m_ProjectionMatrixR, m_AspectRatio, m_FOV, m_Slider3DState, false);
			m_ViewProjectionMatrixR = m_ProjectionMatrixR * m_ViewMatrix; // m_ViewMatrixR (?)
		}

		RecalculateNormals();
	}
	void PerspectiveCamera::RecalculateNormals()
	{
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(m_ViewMatrix)));
		right = glm::normalize(glm::vec3(normalMatrix[0][0], normalMatrix[1][0], normalMatrix[2][0]));
		up = glm::normalize(glm::vec3(normalMatrix[0][1], normalMatrix[1][1], normalMatrix[2][1]));
		forward = glm::normalize(glm::vec3(-normalMatrix[0][2], -normalMatrix[1][2], -normalMatrix[2][2]));
	}
}
