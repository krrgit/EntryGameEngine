#include "etpch.h"
#include "PerspectiveCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Entry {
	
	PerspectiveCamera::PerspectiveCamera(float left, float right, float bottom, float top)
	{
		Mtx_PerspTilt(&m_ProjectionMatrix, C3D_AngleFromDegrees(80.0f), C3D_AspectRatioBot, 0.01f, 1000.0f, false);
		Mtx_Identity(&m_ViewMatrix);
		Mtx_Translate(&m_ViewMatrix, 0.0f, 0.0f, 1.0f, true);
		Mtx_Multiply(&m_ViewProjectionMatrix, &m_ProjectionMatrix, &m_ViewMatrix);
	}
	void PerspectiveCamera::RecalculateViewMatrix()
	{
		//glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position * 
		//	glm::rotate(glm::mat4(1.0f), 45, glm::vec3(0, 0, 1)));
		
		C3D_Mtx transform;
		Mtx_Identity(&transform);
		Mtx_Translate(&transform, m_Position.x, m_Position.y, m_Position.z, true);
		Mtx_RotateX(&transform, glm::radians(m_Rotation.x), true);
		Mtx_RotateY(&transform, glm::radians(m_Rotation.y), true);
		Mtx_RotateZ(&transform, glm::radians(m_Rotation.z), true);

		Mtx_Inverse(&transform);
		m_ViewMatrix = transform;
		Mtx_Multiply(&m_ViewProjectionMatrix, &m_ViewMatrix, &m_ProjectionMatrix);
	}
}