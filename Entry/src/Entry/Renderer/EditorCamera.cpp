// Source: https://github.com/TheCherno/Hazel
#include "etpch.h"

#ifdef ET_PLATFORM_WINDOWS
#include "EditorCamera.h"

#include "Entry/Core/Input.h"
#include "Entry/Core/KeyCodes.h"
#include "Entry/Core/MouseCodes.h"

#include "PerspectiveCamera.h"

#include <glfw/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

namespace Entry {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), Camera(PerspectiveCamera::CalculateProjection(m_FOV, m_AspectRatio, m_NearClip, m_FarClip))
	{
		UpdateView();
	}

	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = PerspectiveCamera::CalculateProjection(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::UpdateView()
	{
		// m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation); //TODO: Replace glm::ToMat4() to remove GLM_ENABLE_EXPERIMENTAL
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	glm::vec2 EditorCamera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::RotationSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::min(distance, 0.0f);
		float speed = distance * distance;
		speed = std::max(speed, 100.0f); // max speed = 100
		return speed;
	}

	void EditorCamera::OnUpdate(Timestep ts)
	{
		bool mouseButtonPressed = (Input::IsMouseButtonPressed(Mouse::ButtonLeft) || Input::IsMouseButtonPressed(Mouse::ButtonMiddle) || Input::IsMouseButtonPressed(Mouse::ButtonRight));
		if (!m_StartMoving && mouseButtonPressed)
		{
			m_StartMoving = true;
			m_InitialMousePosition = { Input::GetMouseX(), Input::GetMouseY() };
		} else if (m_StartMoving && !mouseButtonPressed)
		{ 
			m_StartMoving = false;
		}

		const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		glm::vec2 delta = (m_InitialMousePosition - mouse) * 0.003f;
		m_InitialMousePosition = mouse;

		if (Input::IsKeyPressed(Key::LeftAlt))
		{
			if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
				MouseRotate(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
				MouseZoom(delta.y);
		}
		else
		{
			if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
				MousePan(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
				MouseLookAround(delta);
		}

		// Reset Focal Point
		if (m_ResetFocalPoint && !Input::IsMouseButtonPressed(Mouse::ButtonRight))
		{
			m_ResetFocalPoint = false;
			m_Distance = m_InitialDistance;
			m_FocalPoint = m_Position + (GetForwardDirection() * m_Distance);
		}

		UpdateView();
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ET_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * -0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		auto speed = PanSpeed();

		m_FocalPoint += -GetRightDirection() * delta.x * speed.x * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * speed.y * m_Distance;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseLookAround(const glm::vec2& delta) 
	{
		if (!m_ResetFocalPoint)
		{
			// Store
			m_ResetFocalPoint = true;
			m_InitialDistance = m_Distance;
			m_InitialFocalPoint = m_FocalPoint;
			// Set
			m_Distance = 0.0f;
			m_FocalPoint = m_Position;
		}
		MouseRotate(delta);
	}

	void EditorCamera::MouseZoom(float delta)
	{
		m_Distance += delta * (m_Distance * 0.5f);
		if (m_Distance > -0.1f)
		{
			m_FocalPoint -= GetForwardDirection() * 5.0f;
			m_Distance = -5.0f;
		}
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

}
#endif // ET_PLATFORM_WINDOWS