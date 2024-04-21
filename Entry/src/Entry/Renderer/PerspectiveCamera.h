#pragma once

#include<citro3d.h>
#include<glm/glm.hpp>

namespace Entry {
	class PerspectiveCamera {
	public:
		PerspectiveCamera(float left, float right, float bottom, float top);

		const glm::vec3 GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
		
		const glm::vec4 GetRotation() const { return m_Rotation; }
		void SetRotation(const glm::vec4& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
		 
		const C3D_Mtx* GetProjectionMatrix() const { return &m_ProjectionMatrix; }
		const C3D_Mtx GetViewMatrix() const { return m_ViewMatrix; }
		const C3D_Mtx* GetViewProjectionMatrix() const { return &m_ViewProjectionMatrix; }
	private:
		void RecalculateViewMatrix();
	private:
		C3D_Mtx m_ProjectionMatrix;
		C3D_Mtx m_ViewMatrix;
		C3D_Mtx m_ViewProjectionMatrix;

		glm::vec3 m_Position = {0.0f, 0.0f, 1.0f};
		glm::vec4 m_Rotation;
	};
}