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

	void Citro3DLightEnvironment::BindMaterial(Material* material)
	{
		C3D_Material* mat = reinterpret_cast<C3D_Material*>(&material->GetProps().Values);
		C3D_LightEnvMaterial(&m_LightEnv, mat);

		// Set Used TexEnv
		int i = 0;
		int texEnvCount = 6;// material->TexEnvCount();
		for (; i < texEnvCount; i++)
		{
			SetTexEnv(i, material->GetTexEnvProps(i));
		}

		// Reset Unused TexEnvs
		for (; i < 6; i++)
		{
			C3D_TexEnvInit(C3D_GetTexEnv(i));
		}
	}

	void Citro3DLightEnvironment::SetTexEnv(int id, const TexEnvProps& texEnvProps)
	{
		C3D_TexEnv* env = C3D_GetTexEnv(id);

		switch (texEnvProps.Channels)
		{
		case TexEnvChannels::ET_RGBA:
		{
			C3D_TexEnvSrc(
				env,
				(C3D_TexEnvMode)texEnvProps.Channels,
				(GPU_TEVSRC)texEnvProps.Source1,
				(GPU_TEVSRC)texEnvProps.Source2,
				(GPU_TEVSRC)texEnvProps.Source3
			);
			C3D_TexEnvFunc(
				env,
				(C3D_TexEnvMode)texEnvProps.Channels,
				(GPU_COMBINEFUNC)texEnvProps.BlendMode
			);
		}
		break;
		case TexEnvChannels::ET_Alpha:
		{
			// Reset RGB
			C3D_TexEnvSrc(
				env,
				C3D_RGB,
				GPU_PREVIOUS,
				GPU_PRIMARY_COLOR,
				GPU_PRIMARY_COLOR
			);
			C3D_TexEnvFunc(
				env,
				C3D_RGB,
				GPU_REPLACE
			);

			// Set Alpha
			C3D_TexEnvSrc(
				env,
				C3D_Alpha,
				(GPU_TEVSRC)texEnvProps.AlphaSource1,
				(GPU_TEVSRC)texEnvProps.AlphaSource2,
				(GPU_TEVSRC)texEnvProps.AlphaSource3
			);
			C3D_TexEnvFunc(
				env,
				C3D_Alpha,
				(GPU_COMBINEFUNC)texEnvProps.AlphaBlendMode
			);
		}
		break;
		case TexEnvChannels::ET_RGB:
		{
			// Set RGB
			C3D_TexEnvSrc(
				env,
				C3D_RGB,
				(GPU_TEVSRC)texEnvProps.Source1,
				(GPU_TEVSRC)texEnvProps.Source2,
				(GPU_TEVSRC)texEnvProps.Source3
			);
			C3D_TexEnvFunc(
				env,
				C3D_RGB,
				(GPU_COMBINEFUNC)texEnvProps.BlendMode
			);

			// Reset Alpha
			C3D_TexEnvSrc(
				env,
				C3D_Alpha,
				GPU_PREVIOUS,
				GPU_PRIMARY_COLOR,
				GPU_PRIMARY_COLOR
			);
			C3D_TexEnvFunc(
				env,
				C3D_Alpha,
				GPU_REPLACE
			);
		}
		break;
		case TexEnvChannels::ET_RGBA_Separate:
		{
			// Set RGB
			C3D_TexEnvSrc(
				env,
				C3D_RGB,
				(GPU_TEVSRC)texEnvProps.Source1,
				(GPU_TEVSRC)texEnvProps.Source2,
				(GPU_TEVSRC)texEnvProps.Source3
			);
			C3D_TexEnvFunc(
				env,
				C3D_RGB,
				(GPU_COMBINEFUNC)texEnvProps.BlendMode
			);

			// Set Alpha
			C3D_TexEnvSrc(
				env,
				C3D_Alpha,
				(GPU_TEVSRC)texEnvProps.AlphaSource1,
				(GPU_TEVSRC)texEnvProps.AlphaSource2,
				(GPU_TEVSRC)texEnvProps.AlphaSource3
			);
			C3D_TexEnvFunc(
				env,
				C3D_Alpha,
				(GPU_COMBINEFUNC)texEnvProps.AlphaBlendMode
			);
		}
		break;
		default:
		break;
		}
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
		if (config.funcType == LutFuncType::None) return;

		int ids[] = { 0, 1, -1, 2, 3, 4, 5,-1 };
		int id = ids[config.id];
		if (id > -1)
		{
			//LightLut lut;

			switch (config.funcType)
			{
			case LutFuncType::Pow:
			ET_LightLut_Phong((LightLut*) & m_Luts[id], config.funcArgs.powExponent);
			break;
			case LutFuncType::Spotlight:
			LightLut_Spotlight(&m_Luts[id], config.funcArgs.spotlightCutoff);
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
			return; // Do Nothing
			//ET_LightLut_Zeroes((LightLut*)&m_Luts[id], 0.0f);
			break;
			}

			//m_Luts[id] = lut;

			// Upload
			C3D_LightEnvLut(&m_LightEnv, (GPU_LIGHTLUTID)config.id, (GPU_LIGHTLUTINPUT)config.input, config.negative, &m_Luts[id]);
		}

	}
}
