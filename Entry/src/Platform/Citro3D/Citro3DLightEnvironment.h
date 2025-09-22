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

		virtual int LightInit(Ref<Light> light) override;
		virtual void LightDestroy(Ref<Light> light) override;

		virtual int GetLightCount() override { return m_LightCount; }

		virtual void SetSceneAmbientColor(glm::vec3 color) override;
		virtual glm::vec3 GetSceneAmbientColor() override { return glm::vec3(m_LightEnv.ambient[0], m_LightEnv.ambient[1], m_LightEnv.ambient[2]); }

		virtual void ConfigureLut(LutConfig& config) override;
		virtual LutConfig& GetLutConfig(ET_LIGHTLUTID id) override
		{
			static int ids[] = { 0, 1, -1, 2, 3, 4, 5,-1 };
			return m_LutConfigs[ids[id]];
		}

	private:
		C3D_LightEnv m_LightEnv;
		Ref<Material> m_Material;

		uint16_t m_LightCount = 0;
	
		C3D_LightLut m_Luts[6];
		LutConfig m_LutConfigs[6];
	};
}