#pragma once

#include "Entry/Renderer/Camera.h"

namespace Entry 
{
	class SceneCamera : public Camera {
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetPerspective(float fov, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveFOV(float fov) { m_PerspectiveFOV = fov; RecalculateProjection(); }
	private:
		void RecalculateProjection();
	private:
		float m_PerspectiveFOV = 80.0f;
		float m_PerspectiveNear = 0.001f, m_PerspectiveFar = 1000.0f;
		float m_AspectRatio = 0.0f;
	};
}