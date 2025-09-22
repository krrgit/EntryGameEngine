#include "Citro3DLightEnvironment.h"

namespace Entry
{
	Citro3DLightEnvironment::Citro3DLightEnvironment()
	{
		C3D_LightEnvInit(&m_LightEnv);

		ET_LIGHTLUTID ids[] = { ET_LUT_D0, ET_LUT_D1, ET_LUT_FR, ET_LUT_RB, ET_LUT_RG, ET_LUT_RR };

		for (int i = 0; i < 6; i++)
		{
			m_LutConfigs[i].id = ids[i];
		}
	}

	void Citro3DLightEnvironment::Bind()
	{
		C3D_LightEnvBind(&m_LightEnv);
	}

	void Citro3DLightEnvironment::SetMaterial(Ref<Material> material)
	{
		C3D_Material* mat = reinterpret_cast<C3D_Material*>(&material->GetProps().Values);
		C3D_LightEnvMaterial(&m_LightEnv, mat);
	}

	int Citro3DLightEnvironment::LightInit(Ref<Light> light)
	{
		int id = C3D_LightInit((C3D_Light*)light->GetNativeLight(), &m_LightEnv);
		if (id == -1) return -1;

		light->SetID(id);
		light->SetParent((uint32_t)this);

		light->SetupLight();

		m_LightCount++;
		return 1;
	}

	void Citro3DLightEnvironment::LightDestroy(Ref<Light> light)
	{
		if (light->GetID() == -1) return;

		m_LightCount--;
	}

	void Citro3DLightEnvironment::SetSceneAmbientColor(glm::vec3 color)
	{
		C3D_LightEnvAmbient(&m_LightEnv, color.r, color.g, color.b);
	}

	void Citro3DLightEnvironment::ConfigureLut(LutConfig& config)
	{
		printf("CONFIGURE LUT\n");

		//static C3D_LightLut lut_Phong;
		//LightLut_Phong(&lut_Phong, 1000.0f);
		//LightLut_Phong(&m_Luts[0], 1000.0f);

		//m_Luts[0] = lut_Phong;
		//ET_LightLut_Phong((LightLut*) & m_Luts[0], 1000.0f);
		//C3D_LightEnvLut(&m_LightEnv, GPU_LUT_D0, GPU_LUTINPUT_NH, false, &m_Luts[0]);
		int ids[] = { 0, 1, -1, 2, 3, 4, 5,-1 };
		int id = ids[config.id];
		if (id > -1)
		{
			//LightLut lut;

			switch (config.funcType)
			{
			case LutFuncType::Pow:
			ET_LightLut_Phong((LightLut*) & m_Luts[id], config.funcArgs.powExponent);
			printf("config pow");
			break;
			case LutFuncType::Spotlight:
			LightLut_Spotlight(&m_Luts[id], config.funcArgs.spotlightCutoff);
			break;
			case LutFuncType::Quadratic:
			//LightLutDA_Quadratic(&lut,)
			break;
			case LutFuncType::ToonDiffuse:
			ET_LightLut_ToonDiffuse((LightLut*) & m_Luts[id], 0.0f);
			break;
			case LutFuncType::ToonSpecular:
			ET_LightLut_ToonDiffuse((LightLut*)&m_Luts[id], config.funcArgs.toonShininess);
			break;
			case LutFuncType::Custom:
			break;
			default:
			ET_LightLut_Zeroes((LightLut*)&m_Luts[id], 0.0f);
			break;
			}

			//m_Luts[id] = lut;

			// Upload
			C3D_LightEnvLut(&m_LightEnv, (GPU_LIGHTLUTID)config.id, (GPU_LIGHTLUTINPUT)config.input, config.negative, &m_Luts[id]);
			//C3D_LightEnvLut(&m_LightEnv, (GPU_LIGHTLUTID)config.id, (GPU_LIGHTLUTINPUT)config.input, config.negative, &m_Luts[id]);

		}

	}
}
