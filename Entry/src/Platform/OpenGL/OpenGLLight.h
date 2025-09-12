#pragma once

#include "Entry/Renderer/Light.h"

namespace Entry
{
	class OpenGLLight : public Light
	{
	public:
		OpenGLLight(LightProps& props);
		virtual ~OpenGLLight();

		virtual void SetStrength(float strength) override { m_Strength = strength; }
		virtual void SetAngle(float angle) override { m_Angle = angle; }
		virtual void SetColor(glm::vec3 color) override { m_Color = color; }
		virtual void SetPosition(glm::vec3 position) override;
	private:
		void UpdateLight();
	private:
		LightType m_LightType;
		float m_Strength;
		float m_Angle;
		glm::vec3 m_Color;
	};
}