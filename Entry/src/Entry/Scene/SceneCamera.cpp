#include "etpch.h"

#include "SceneCamera.h"
#include "Entry/Renderer/PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Entry {
	SceneCamera::SceneCamera()
	{
		//SetRenderTarget(ET_GFX_SCREEN::GFX_TOP);
	}

	void SceneCamera::SetRenderTarget(ET_GFX_SCREEN screen)
	{
		switch (screen)
		{
		case ET_GFX_SCREEN::GFX_TOP:
		{
			ET_PROFILE_SCOPE("Create Top Framebuffer");
			m_Framebuffer.reset();
			FramebufferSpecification frameBufSpec;
			frameBufSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
			frameBufSpec.Width = 400;
			frameBufSpec.Height = 240;
			frameBufSpec.Screen = ET_GFX_SCREEN::GFX_TOP;
			frameBufSpec.Side = ET_GFX_3D_SIDE::GFX_LEFT;
			m_Framebuffer = Entry::Framebuffer::Create(frameBufSpec);
			m_AspectRatio = 400.0f / 240.0f;
			RecalculateProjection();
			break;
		}
		case ET_GFX_SCREEN::GFX_BOTTOM:
		{
			ET_PROFILE_SCOPE("Create Bottom Framebuffer");
			m_Framebuffer.reset();
			FramebufferSpecification frameBufSpec;
			frameBufSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
			frameBufSpec.Width = 320;
			frameBufSpec.Height = 240;
			frameBufSpec.Screen = ET_GFX_SCREEN::GFX_BOTTOM;
			frameBufSpec.Side = ET_GFX_3D_SIDE::GFX_LEFT;
			m_Framebuffer = Entry::Framebuffer::Create(frameBufSpec);
			m_AspectRatio = 320.0f / 240.0f;
			RecalculateProjection();
			break;
		}
		default:
			m_Framebuffer.reset();
			break;
		}

		ET_CORE_ASSERT("Invalid render target for framebuffer");
	}

	void SceneCamera::SetPerspective(float fov, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Persepective;
		m_PerspectiveFOV = fov;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;

		RecalculateProjection();
	}
	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjection();
	}
	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}
	void SceneCamera::RecalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Persepective)
		{
			// TODO: support stereo 3D 
			m_Projection = PerspectiveCamera::CalculateProjection(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
		else
		{
			//float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
			//float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
			//float orthoBottom = -m_OrthographicSize * 0.5f;
			//float orthoTop = -m_OrthographicSize * 0.5f;
			//m_Projection = glm::ortho(orthoLeft, orthoBottom, orthoBottom, orthoTop, -1.0f, 1.0f);
			m_Projection = PerspectiveCamera::CalculateOrthographic(m_OrthographicSize, m_AspectRatio);
		}
	}
}