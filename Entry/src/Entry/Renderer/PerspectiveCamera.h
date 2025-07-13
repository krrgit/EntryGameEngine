#pragma once
#include <3ds.h>
#include<citro3d.h>
#include<glm/glm.hpp>

namespace Entry {
	class PerspectiveCamera {
	public:
		PerspectiveCamera(float left, float right, float bottom, float top);
		void SetProjection(float left, float right, float bottom, float top);

		const glm::vec3 GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix();
			//PrintViewMatrix();
		}
		
		const glm::vec4 GetRotation() const { return m_Rotation; }
		void SetRotation(const glm::vec4& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		 
		const C3D_Mtx* GetProjectionMatrix() const { return &m_ProjectionMatrix; }
		const C3D_Mtx GetViewMatrix() const { return m_ViewMatrix; }
		const C3D_Mtx* GetViewProjectionMatrix() const { return &m_ViewProjectionMatrix; }
	private:
		void RecalculateViewMatrix();

		void PrintViewMatrix() {
			consoleClear();
			printf("%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n%.1f %.1f %.1f %.1f\n",
				m_ViewMatrix.m[0],m_ViewMatrix.m[1], m_ViewMatrix.m[2],m_ViewMatrix.m[3],
				m_ViewMatrix.m[4],m_ViewMatrix.m[5],m_ViewMatrix.m[6],m_ViewMatrix.m[7],
				m_ViewMatrix.m[8],m_ViewMatrix.m[9],m_ViewMatrix.m[10],m_ViewMatrix.m[11],
				m_ViewMatrix.m[12], m_ViewMatrix.m[13], m_ViewMatrix.m[14], m_ViewMatrix.m[15]
			);
		}

	public:
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;
	private:
		C3D_Mtx m_ProjectionMatrix;
		C3D_Mtx m_ViewMatrix;
		C3D_Mtx m_ViewProjectionMatrix;

		glm::vec3 m_Position = {0.0f, 0.0f, 1.0f};
		glm::vec4 m_Rotation;
	};
}