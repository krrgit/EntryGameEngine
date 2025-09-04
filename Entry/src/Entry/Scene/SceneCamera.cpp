#include "etpch.h"

#include "SceneCamera.h"
#include "Entry/Renderer/PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Entry {
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float fov, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Persepective;
		m_PerspectiveFOV = fov;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;

		RecalculateProjection();
	}
	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjection();
	}
	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}
	void SceneCamera::RecalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Persepective)
		{
			// TODO: support stereo 3D 
			m_Projection = PerspectiveCamera::CalculateProjection(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
		else
		{
			//float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
			//float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
			//float orthoBottom = -m_OrthographicSize * 0.5f;
			//float orthoTop = -m_OrthographicSize * 0.5f;
			//m_Projection = glm::ortho(orthoLeft, orthoBottom, orthoBottom, orthoTop, -1.0f, 1.0f);
			m_Projection = PerspectiveCamera::CalculateOrthographic(m_OrthographicSize, m_AspectRatio);
		}
	}
}