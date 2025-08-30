#include "etpch.h"

#include "SceneCamera.h"
#include "Entry/Renderer/PerspectiveCamera.h"

namespace Entry {
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float fov, float nearClip, float farClip)
	{
		m_PerspectiveFOV = fov;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;

		RecalculateProjection();
	}
	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}
	void SceneCamera::RecalculateProjection()
	{
		PerspectiveCamera::CalculateProjection(m_Projection, m_AspectRatio, m_PerspectiveFOV, m_PerspectiveNear, m_PerspectiveFar);
	}
}