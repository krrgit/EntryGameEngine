#pragma once

#include "Entry/Renderer/LightEnvironment.h"

#include <citro3d.h>

namespace Entry
{
	class Citro3DLightEnvironment : public LightEnvironment
	{
	public:
		Citro3DLightEnvironment();
		~Citro3DLightEnvironment() = default;
		virtual void Bind() override;
		virtual void SetMaterial(Ref<Material> material) override;

		virtual void CreatePhongLut(float shininess) override;
		virtual void CreateSpotlightLut(float angle) override;
		virtual void CreateQuadraticLut(float from, float to, float linear, float quad) override;
		virtual void SetLut(ET_LIGHTLUTID id, ET_LIGHTLUTINPUT inputs, bool negative = false) override;

		virtual int LightInit(Ref<Light> light) override;
		virtual void LightDestroy(Ref<Light> light) override;

		virtual int GetLightCount() override { return m_LightCount; }

		virtual void SetSceneAmbientColor(glm::vec3 color) override;
		virtual glm::vec3 GetSceneAmbientColor() override { return glm::vec3(m_LightEnv.ambient[0], m_LightEnv.ambient[1], m_LightEnv.ambient[2]); }

	private:
		C3D_LightEnv m_LightEnv;
		C3D_LightLut m_Lut;
		C3D_LightLutDA m_LutDA;
		Ref<Material> m_Material;

		uint16_t m_LightCount = 0;
	};
}