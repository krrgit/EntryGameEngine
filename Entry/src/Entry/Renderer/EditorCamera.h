// Source (Modified): https://github.com/TheCherno/Hazel
#pragma once

#include "Camera.h"
#include "Entry/Core/Timestep.h"
#include "Entry/Events/Event.h"

#ifdef ET_PLATFORM_WINDOWS
#include "Entry/Events/MouseEvent.h"
#endif // ET_PLATFORM_WINDOWS

#include <glm/glm.hpp>

namespace Entry
{

	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }
		inline glm::vec3 GetFocalPoint() const { return m_FocalPoint; }

		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const;

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }
	private:
		void UpdateProjection();
		void UpdateView();

#ifdef ET_PLATFORM_WINDOWS
		bool OnMouseScroll(MouseScrolledEvent& e);
#endif // ET_PLATFORM_WINDOWS

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseLookAround(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;

		glm::vec2 PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;
	private:
		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		bool m_StartMoving = false;

		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f }; // Calculated from m_FocalPoint * m_Distance
		glm::vec3 m_FocalPoint = { 0.0f, 1.0f, 0.0f };

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = -10.0f;
		float m_Pitch = -0.35f, m_Yaw = 0.0f;

		float m_ViewportWidth = 1280, m_ViewportHeight = 720;

		bool m_ResetFocalPoint = false;
		float m_InitialDistance = -10.0f;
		glm::vec3 m_InitialFocalPoint{ 0.0f };
	};

}
