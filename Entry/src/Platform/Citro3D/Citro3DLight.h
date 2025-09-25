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

		virtual void SetID(int id) override { m_LightID = id; }
		virtual int GetID() override { return m_LightID; }

		virtual void SetLight(LightProps props) override;
		virtual void* GetNativeLight() override { return (void*) & m_Light; };

		virtual void SetParent(uint32_t parent) override { m_Parent = (C3D_LightEnv*)parent; }

		virtual void SetAsDirectionalLight() override;
		virtual void SetAsPointLight(float linear, float quad) override;
		virtual void SetAsSpotLight(float angle) override;

		virtual void SetupLight() override;

	private:
		LightType m_LightType;
		glm::vec3 m_Direction;
		float m_Strength;
		float m_Angle;
		glm::vec3 m_Color;
		float m_PositionalLight = 0.0f; // Really, is just a bool


		int m_LightID = -1;
		C3D_Light m_Light;
		C3D_LightLut m_Lut;
		C3D_LightLutDA m_LutDA;
		C3D_FVec m_Position;
		C3D_LightEnv* m_Parent;
	};
}