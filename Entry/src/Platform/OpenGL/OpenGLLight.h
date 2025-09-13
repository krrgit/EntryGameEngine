#pragma once

#include "Entry/Renderer/Light.h"

namespace Entry
{
	class OpenGLLight : public Light
	{
	public:
		OpenGLLight(LightProps& props);
		virtual ~OpenGLLight();

		virtual void SetLight(LightProps props) override;
	private:
		LightType m_LightType;
		float m_Strength;
		float m_Angle;
		glm::vec3 m_Color;
	};
}