#pragma once

#include "Entry/Renderer/PerspectiveCamera.h"
#include "Entry/Core/Timestep.h"

#include "Entry/Events/ApplicationEvent.h"
#include "Entry/Events/KeyEvent.h"

#include <glm/gtc/type_ptr.hpp>

namespace Entry {
	class PerspectiveCameraController
	{
	public:
		PerspectiveCameraController(float aspectRatio, bool rotation = true); 

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		PerspectiveCamera& GetCamera() { return m_Camera; }
		const PerspectiveCamera& GetCamera() const { return m_Camera; }

	private:
		// Events ?
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		PerspectiveCamera m_Camera;

		bool m_Rotation;
		float m_Slider3DState, m_Slider3DStatePrev;

		glm::vec3 m_CamPos = { 0.0f, 1.0f, 1.0f };
		glm::vec4 m_CamRot = { 0.0f, 0.0f, 0.0f, 0.0f };
		float m_CameraTranslationSpeed = 3.0f;
		float m_CameraVertSpeed = 3.0f;
		float m_CameraRotationSpeed = 120.0f;
	};
}