#include "etpch.h"
#include "PerspectiveCamera.h"

#include "glm/gtc/matrix_transform.hpp"

#include "Renderer.h"

namespace Entry {

	void Orthographic(glm::mat4& mtx, float size, float aspectRatio, float nearClip, float farClip, bool isLeftHanded)
	{
		mtx = glm::mat4(0);

		float orthoLeft = -size * aspectRatio * 0.5f;
		float orthoRight = size* aspectRatio * 0.5f;
		float orthoBottom = -size * 0.5f;
		float orthoTop = size * 0.5f;

		// Standard orthogonal projection matrix, with a fixed depth range of [-1,0] (required by PICA)
		mtx[0][0] = 2.0f / (orthoRight - orthoLeft);
		mtx[0][3] = (orthoLeft + orthoRight) / (orthoLeft - orthoRight);
		mtx[1][1] = 2.0f / (orthoTop - orthoBottom);
		mtx[1][3] = (orthoBottom + orthoTop) / (orthoBottom - orthoTop);
		if (isLeftHanded)
			mtx[2][2] = 1.0f / (farClip - nearClip);
		else
			mtx[2][2] = 1.0f / (nearClip - farClip);
		mtx[2][3] = 0.5f * (nearClip + farClip) / (nearClip - farClip) - 0.5f;
		mtx[3][3] = 1.0f;
	}

	void OrthographicTilt(glm::mat4& mtx, float size, float aspectRatio, float nearClip, float farClip, bool isLeftHanded)
	{
		mtx = glm::mat4(0);

		float orthoLeft = size* 0.5f;
		float orthoRight = -size * 0.5f;
		float orthoBottom = size * aspectRatio * 0.5f;
		float orthoTop = -size * aspectRatio * 0.5f;

		// Standard orthogonal projection matrix, with a fixed depth range of [-1,0] (required by PICA) and rotated τ/4 radians counterclockwise around the Z axis (due to 3DS screen orientation)
		mtx[0][1] = 2.0f / (orthoTop - orthoBottom);
		mtx[0][3] = (orthoBottom + orthoTop) / (orthoBottom - orthoTop);
		mtx[1][0] = 2.0f / (orthoLeft - orthoRight);
		mtx[1][3] = (orthoLeft + orthoRight) / (orthoRight - orthoLeft);
		if (isLeftHanded)
			mtx[2][2] = 1.0f / (farClip - nearClip);
		else
			mtx[2][2] = 1.0f / (nearClip - farClip);
		mtx[2][3] = 0.5f * (nearClip + farClip) / (nearClip - farClip) - 0.5f;
		mtx[3][3] = 1.0f;
	}

	void PerspectiveStereo(glm::mat4& mtx, float fovy, float aspect, float nearClip, float farClip, float iod, float screen, bool isLeftHanded)
	{
		float fovy_rad = glm::radians(fovy);
		float fovy_tan = tanf(fovy_rad / 2.0f);
		float fovy_tan_aspect = fovy_tan * aspect;
		float shift = iod / (2.0f * screen); // 'near' not in the numerator because it cancels out in mp[1].z

		mtx = glm::mat4(0);

		mtx[0][0] = 1.0f / fovy_tan_aspect;
		mtx[3][0] = -iod / 2.0f;
		mtx[1][1] = 1.0f / fovy_tan;
		mtx[3][2] = nearClip * farClip / (nearClip - farClip);
		mtx[2][3] = isLeftHanded ? 1.0f : -1.0f;
		mtx[2][0] = mtx[2][3] * shift / fovy_tan_aspect;
		mtx[2][2] = mtx[2][3] * (farClip + nearClip) / (farClip - nearClip);
	}

	void PersptiveStereoTilt(glm::mat4& mtx, float fovx, float invaspect, float nearClip, float farClip, float iod, float screen, bool isLeftHanded)
	{
		// Notes:
		// Once again, we are passed "fovy" and the "aspect ratio"; however the 3DS screens are sideways,
		// and the formula had to be tweaked. With stereo, left/right separation becomes top/bottom separation.
		// The detailed mathematical explanation is in mtx_persptilt.c.

		float fovx_rad = glm::radians(fovx);
		float fovx_tan = tanf(fovx_rad / 2.0f);
		float fovx_tan_invaspect = fovx_tan * invaspect;
		float shift = iod / (2.0f * screen); // 'near' not in the numerator because it cancels out in mp[1][2]

		mtx = glm::mat4(0);

		mtx[1][0] = 1.0f / fovx_tan;
		mtx[0][1] = -1.0f / fovx_tan_invaspect;
		mtx[3][1] = iod / 2.0f;
		mtx[3][2] = nearClip * farClip / (nearClip - farClip);
		mtx[2][3] = isLeftHanded ? 1.0f : -1.0f;
		mtx[2][1] = -mtx[2][3] * shift / fovx_tan_invaspect;
		mtx[2][2] = -mtx[2][3] * (nearClip) / (farClip - nearClip); // 3DS Specific
	}

	PerspectiveCamera::PerspectiveCamera(float _aspectRatio, float _fov)
	{
		m_ViewMatrix = glm::lookAt(
			m_Position, // Camera position in World Space
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


	glm::mat4 PerspectiveCamera::CalculateProjection(float fov, float aspectRatio, float nearClip, float farClip, float iod, bool isLeftHanded)
	{
		glm::mat4 out(1.0f);
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::Citro3D: // FOR 3DS PLATFORM
			PersptiveStereoTilt(out, fov, aspectRatio, nearClip, farClip, iod, 2.0f, isLeftHanded);
			break;
		default: // For OTHER PLATFORMS/EDITOR (PC)
			PerspectiveStereo(out, fov, aspectRatio, nearClip, farClip, iod, 2.0f, isLeftHanded);
			break;
		}
		return out;
	}

	glm::mat4 PerspectiveCamera::CalculateOrthographic(float size, float aspectRatio, float nearClip, float farClip, bool isLeftHanded)
	{
		glm::mat4 out(1.0f);

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::Citro3D: // FOR 3DS PLATFORM
			OrthographicTilt(out, size, aspectRatio, nearClip, farClip, isLeftHanded);
			break;
		default: // For OTHER PLATFORMS/EDITOR (PC)
			Orthographic(out, size, aspectRatio, nearClip, farClip, isLeftHanded);
			break;
		}
		return out;
	}

	void PerspectiveCamera::RecalculateViewMatrix()
	{
		ET_PROFILE_FUNCTION();
		// m_Rotation in radians
		glm::mat4 transform =	glm::translate(glm::mat4(1.0f), m_Position) *
								glm::rotate(glm::mat4(1.0f), m_Rotation.z, glm::vec3(0,0,1)) *
								glm::rotate(glm::mat4(1.0f), m_Rotation.y, glm::vec3(0,1,0)) *
								glm::rotate(glm::mat4(1.0f), m_Rotation.x, glm::vec3(1,0,0));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

		RecalculateNormals();
	}

	void PerspectiveCamera::RecalculateProjectionViewMatrix()
	{
		m_ProjectionMatrix = CalculateProjection(m_AspectRatio, m_FOV, -m_Slider3DState);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

		// 3DS Only
		if (m_Slider3DState > 0.0f)
		{
			// Right Side
			m_ProjectionMatrixR = CalculateProjection(m_AspectRatio, m_FOV, m_Slider3DState);
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
