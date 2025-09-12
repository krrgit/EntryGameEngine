#pragma once

#include "Entry/Renderer/Light.h"

#include <citro3d.h>

namespace Entry
{
	class Citro3DLight : public Light
	{
	public:
		Citro3DLight(LightProps& props);
		virtual ~Citro3DLight();

		virtual void SetStrength(float strength) override { m_Strength = strength; LightLut_Phong(&m_Lut, m_Strength); }
		virtual void SetAngle(float angle) override { m_Angle = angle; LightLut_Spotlight(&m_Lut, m_Angle); }
		virtual void SetColor(glm::vec3 color) override { m_Color = color; C3D_LightColor(&m_Light, m_Color.r, m_Color.g, m_Color.b); }
		virtual void SetPosition(glm::vec3 position) override;
	private:
		void UpdateLight();
	private:
		LightType m_LightType;
		float m_Strength;
		float m_Angle;
		glm::vec3 m_Color;

		C3D_LightEnv m_LightEnv;
		C3D_Light m_Light;
		C3D_LightLut m_Lut;
		C3D_FVec m_Position;
	};
}