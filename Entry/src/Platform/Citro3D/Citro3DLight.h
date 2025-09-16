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

		virtual void SetID(uint16_t id) override {}
		virtual uint16_t GetID() override { return 0; }

		virtual void SetLight(LightProps props) override;
		virtual void* GetNativeLight() override { return (void*) & m_Light; };

		virtual void SetParent(int parent) override { }
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