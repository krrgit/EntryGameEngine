#include "etpch.h"
#include "PerspectiveCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Entry {

	C3D_FVec vec3_to_c3d_fvec(glm::vec3 v) {
		C3D_FVec result = { 1.0f, v.z, v.y, v.x };
		return result;
	}
	
	PerspectiveCamera::PerspectiveCamera(float _left, float _right, float _bottom, float _top)
	{
		// Set View Matrix
		m_ViewMatrix.r[0].x = 2.0f / (_right - _left);
		m_ViewMatrix.r[1].y = 2.0f / (_top - _bottom);
		m_ViewMatrix.r[2].z = -1;
		m_ViewMatrix.r[3].x = (_right + _left) / (_top - _bottom);
		m_ViewMatrix.r[3].y = (_top + _bottom) / (_top - _bottom);

		// Calculate ViewProjection & Projection (Unused)
		Mtx_PerspTilt(&m_ProjectionMatrix, C3D_AngleFromDegrees(80.0f), C3D_AspectRatioBot, 0.01f, 1000.0f, false);
		Mtx_Multiply(&m_ViewProjectionMatrix, &m_ProjectionMatrix, &m_ViewMatrix);


		// Calculate normals
		Mtx_Transpose(&m_ViewMatrix);
		right = glm::vec3(m_ViewMatrix.m[3], m_ViewMatrix.m[7], m_ViewMatrix.m[11]);
		up = glm::vec3(m_ViewMatrix.m[2], m_ViewMatrix.m[6], m_ViewMatrix.m[10]);
		forward = glm::vec3(-m_ViewMatrix.m[1], -m_ViewMatrix.m[5], -m_ViewMatrix.m[9]);
	}
	void PerspectiveCamera::RecalculateViewMatrix()
	{
		C3D_Mtx transform;
		Mtx_Identity(&transform);
		Mtx_Translate(&transform, m_Position.x, m_Position.y, m_Position.z, true);
		Mtx_Rotate(&transform, vec3_to_c3d_fvec(right), glm::radians(m_Rotation.x), true);
		Mtx_RotateY(&transform, glm::radians(m_Rotation.y), true);
		Mtx_RotateZ(&transform, glm::radians(m_Rotation.z), true);

		Mtx_Inverse(&transform);
		m_ViewMatrix = transform;

		// (Unused)
		Mtx_Multiply(&m_ViewProjectionMatrix, &m_ViewMatrix, &m_ProjectionMatrix);

		C3D_Mtx normals;
		Mtx_Copy(&normals, &transform);
		Mtx_Transpose(&normals);
		right = glm::vec3(normals.m[3], normals.m[7], normals.m[11]);
		up = glm::vec3(normals.m[2], normals.m[6], normals.m[10]);
		forward = glm::vec3(-normals.m[1], -normals.m[5], -normals.m[9]);
	}
}