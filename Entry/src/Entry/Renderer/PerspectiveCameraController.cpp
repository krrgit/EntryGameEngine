#include "etpch.h"
#include "PerspectiveCameraController.h"
#include "Entry/Core/Input.h"
#include "Entry/Core/KeyCodes.h"

namespace Entry {
	PerspectiveCameraController::PerspectiveCameraController(float aspectRatio, float fov, bool rotation) 
    : m_AspectRatio(aspectRatio), m_Camera(aspectRatio, fov), m_Rotation(rotation) {
	}

	void PerspectiveCameraController::OnUpdate(Timestep ts) {
        ET_PROFILE_FUNCTION();

        glm::vec2 cp = Input::GetJoystickPos();
        m_Slider3DState = Input::GetSlider3D();

        glm::vec3 forward = m_Camera.forward;
        glm::vec3 right = m_Camera.right;
        //forward.y = 0; // uncomment to only move laterally
        //right.y = 0; // uncomment to only move laterally
        m_CamPos = m_CamPos + forward * (cp.y * m_CameraTranslationSpeed * ts) + (right * (cp.x * m_CameraTranslationSpeed * ts));
        int LandR = (Input::GetButton(ET_PAD_R) ? 1 : 0) - (Input::GetButton(ET_PAD_L) ? 1 : 0);
        m_CamPos.y += LandR * m_CameraVertSpeed * ts;

        if (m_Rotation) {
            int cStickX = (Input::GetButton(ET_PAD_CSTICK_LEFT) ? 1 : 0) - (Input::GetButton(ET_PAD_CSTICK_RIGHT) ? 1 : 0);
            int cStickY = (Input::GetButton(ET_PAD_CSTICK_UP) ? 1 : 0) - (Input::GetButton(ET_PAD_CSTICK_DOWN) ? 1 : 0);

            m_CamRot = glm::vec4(m_CamRot.x + (cStickY * m_CameraRotationSpeed * ts), m_CamRot.y + (cStickX * m_CameraRotationSpeed * ts), m_CamRot.z, m_CamRot.w);
        }

        if (m_Slider3DState != m_Slider3DStatePrev)
        {
            m_Camera.Set3DValue(m_Slider3DState);
            m_Slider3DStatePrev = m_Slider3DState;
        }
        m_Camera.SetPosition(m_CamPos);
        m_Camera.SetRotation(m_CamRot);
	}

	void PerspectiveCameraController::OnEvent(Event& e) {
        ET_PROFILE_FUNCTION();

#ifdef ET_PLATFORM_WINDOWS
        EventDispatcher dispatcher(e);
        //dispatcher.Dispatch<MouseScrolledEvent>(ET_BIND_EVENT_FN(PerspectiveCameraController::OnMouseScrolled));
        dispatcher.Dispatch<WindowResizeEvent>(ET_BIND_EVENT_FN(PerspectiveCameraController::OnWindowResized));
#endif // ET_PLATFORM_WINDOWS
       
	}
    void PerspectiveCameraController::OnResize(float width, float height)
    {
        ET_PROFILE_FUNCTION();

        m_AspectRatio = width / height;
        m_Camera.SetAspectRatio(width / height);
    }

#ifdef ET_PLATFORM_WINDOWS
    
    bool PerspectiveCameraController::OnWindowResized(WindowResizeEvent& e)
    {
        ET_PROFILE_FUNCTION();

        OnResize((float)e.GetWidth(), (float)e.GetHeight());

        return false;
    }
#endif
}