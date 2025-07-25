#include "etpch.h"
#include "PerspectiveCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Entry {

	//C3D_FVec vec3_to_c3d_fvec(glm::vec3 v) {
	//	C3D_FVec result = { 1.0f, v.z, v.y, v.x };
	//	return result;
	//}

	PerspectiveCamera::PerspectiveCamera(float _left, float _right, float _bottom, float _top)
	{
		SetProjection(_left, _right, _bottom, _top);

		//// Set View Matrix
		//m_ViewMatrix.r[0].x = 2.0f / (_right - _left);
		//m_ViewMatrix.r[1].y = 2.0f / (_top - _bottom);
		//m_ViewMatrix.r[2].z = -1;
		//m_ViewMatrix.r[3].x = (_right + _left) / (_top - _bottom);
		//m_ViewMatrix.r[3].y = (_top + _bottom) / (_top - _bottom);


		//// Calculate ViewProjection & Projection
		// Mtx_PerspTilt(&m_ProjectionMatrix, C3D_AngleFromDegrees(80.0f), C3D_AspectRatioTop, 0.01f, 1000.0f, false);
		//Mtx_Multiply(&m_ViewProjectionMatrix, &m_ProjectionMatrix, &m_ViewMatrix);

		//// Calculate normals
		//Mtx_Transpose(&m_ViewMatrix);
		//right = glm::vec3(m_ViewMatrix.m[3], m_ViewMatrix.m[7], m_ViewMatrix.m[11]);
		//up = glm::vec3(m_ViewMatrix.m[2], m_ViewMatrix.m[6], m_ViewMatrix.m[10]);
		//forward = glm::vec3(-m_ViewMatrix.m[1], -m_ViewMatrix.m[5], -m_ViewMatrix.m[9]);
	}


	void PerspectiveCamera::SetProjection(float _left, float _right, float _bottom, float _top)
	{	
		m_ViewMatrix = glm::lookAt(
			glm::vec3(0, 0, -1), // Camera position in World Space
			glm::vec3(0, 0, 1), // look direction
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

		//m_ProjectionMatrix = glm::perspective(glm::radians(fov), 400.0f/240.0f, 0.01f, 1000.0f);

		// TODO: Only use when platform is 3DS
		// 3DS screens are sideways. See Mtx_PerspTilt() in mtx_persplit.c for details.
		{
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
		}

		/*m_ProjectionMatrix = {  0,		1.2f,	0,	0,
								-0.7f,	0,		0,	0,
								0,		0,		0,	0,
								0,		0,		-1.0f,	0
		};*/
		
		m_ProjectionMatrix = glm::transpose(m_ProjectionMatrix);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

		// Get Normals
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(m_ViewMatrix));

		right = glm::vec3(normalMatrix[0][0], normalMatrix[0][1], normalMatrix[0][2]);
		up = glm::vec3(normalMatrix[1][0], normalMatrix[1][1], normalMatrix[1][2]);
		forward = glm::vec3(-normalMatrix[2][0], -normalMatrix[2][1], -normalMatrix[2][2]);


		//--------------
	
		// Set View Matrix
		//m_ViewMatrix = glm::mat4(1.0f);
		//m_ViewMatrix.r[0].x = 2.0f / (_right - _left);
		//m_ViewMatrix.r[1].y = 2.0f / (_top - _bottom);
		//m_ViewMatrix.r[2].z = -1;
		//m_ViewMatrix.r[3].x = (_right + _left) / (_top - _bottom);
		//m_ViewMatrix.r[3].y = (_top + _bottom) / (_top - _bottom);

		//// Calculate ViewProjection & Projection
		//Mtx_PerspTilt(&m_ProjectionMatrix, C3D_AngleFromDegrees(80.0f), C3D_AspectRatioTop, 0.01f, 1000.0f, false);
		//Mtx_Multiply(&m_ViewProjectionMatrix, &m_ProjectionMatrix, &m_ViewMatrix);

		//// Calculate normals
		//Mtx_Transpose(&m_ViewMatrix);
		//right = glm::vec3(m_ViewMatrix.m[3], m_ViewMatrix.m[7], m_ViewMatrix.m[11]);
		//up = glm::vec3(m_ViewMatrix.m[2], m_ViewMatrix.m[6], m_ViewMatrix.m[10]);
		//forward = glm::vec3(-m_ViewMatrix.m[1], -m_ViewMatrix.m[5], -m_ViewMatrix.m[9]);
	}


	void PerspectiveCamera::RecalculateViewMatrix()
	{

		// Apply rotation in ZYX order (or any consistent order you expect)
		//glm::mat4 transform = glm::mat4(1.0f);
		//transform = glm::rotate(transform, glm::radians(m_Rotation.x), glm::vec3(1,0,0));
		//transform = glm::rotate(transform, glm::radians(m_Rotation.y), glm::vec3(0,1,0));
		//transform = glm::rotate(transform, glm::radians(m_Rotation.z), glm::vec3(0,0,1));
		//transform = glm::translate(transform, m_Position);

		glm::mat4 transform =	glm::translate(glm::mat4(1.0f), m_Position) *
								glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0,0,1)) *
								glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0,1,0)) *
								glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), right);

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;


		// Get Normals
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(m_ViewMatrix)));
		right = glm::normalize(glm::vec3(normalMatrix[0][0], normalMatrix[0][1], normalMatrix[0][2]));
		up = glm::normalize(glm::vec3(normalMatrix[1][0], normalMatrix[1][1], normalMatrix[1][2]));
		forward = glm::normalize(glm::vec3(-normalMatrix[2][0], -normalMatrix[2][1], -normalMatrix[2][2]));

		consoleClear();

		glm::mat4 glm_in = normalMatrix;
		printf("%.1f %.1f %.1f\n%.1f %.1f %.1f\n%.1f %.1f %.1f\n",
			glm_in[0].x, glm_in[0].y, glm_in[0].z,
			glm_in[1].x, glm_in[1].y, glm_in[1].z,
			glm_in[2].x, glm_in[2].y, glm_in[2].z
		);

		/*C3D_Mtx transform;
		Mtx_Identity(&transform);
		Mtx_Translate(&transform, m_Position.x, m_Position.y, m_Position.z, true);
		Mtx_Rotate(&transform, vec3_to_c3d_fvec(right), glm::radians(m_Rotation.x), true);
		Mtx_RotateY(&transform, glm::radians(m_Rotation.y), true);
		Mtx_RotateZ(&transform, glm::radians(m_Rotation.z), true);

		Mtx_Inverse(&transform);
		m_ViewMatrix = transform;

		Mtx_Multiply(&m_ViewProjectionMatrix, &m_ProjectionMatrix, &m_ViewMatrix);

		C3D_Mtx normals;
		Mtx_Copy(&normals, &transform);
		Mtx_Transpose(&normals);
		right = glm::vec3(normals.m[3], normals.m[7], normals.m[11]);
		up = glm::vec3(normals.m[2], normals.m[6], normals.m[10]);
		forward = glm::vec3(-normals.m[1], -normals.m[5], -normals.m[9]);
		*/
	}
}