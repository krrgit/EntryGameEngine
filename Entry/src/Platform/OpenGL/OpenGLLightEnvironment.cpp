#include "etpch.h"
#include "OpenGLLightEnvironment.h"

namespace Entry
{
	OpenGLLightEnvironment::OpenGLLightEnvironment()
	{
		glGenBuffers(1, &m_LightEnvUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, m_LightEnvUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOLightEnvData), nullptr, GL_DYNAMIC_DRAW);

		UBOLightEnvData defaultValues = {};
		defaultValues.sceneAmbient = glm::vec4(1.0f); // default scene ambient = white
		for(int i =0; i< MAX_LUTS; ++i)
			defaultValues.params[i] = {0,0,90,0};

		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UBOLightEnvData), &defaultValues); // Set to 0
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_LightEnvUBO); // binding = 0 matches shader
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		m_MaterialData =
		{
			{ 0.2f, 0.2f, 0.2f, 0.0f }, //ambient
			{ 0.4f, 0.4f, 0.4f, 0.0f }, //diffuse
			{ 0.8f, 0.8f, 0.8f, 0.0f }, //specular0
			{ 0.0f, 0.0f, 0.0f, 0.0f }, //specular1
			{ 0.0f, 0.0f, 0.0f, 0.0f }, //emission
		};

		glGenBuffers(1, &m_MaterialUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, m_MaterialUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOMaterialData), &m_MaterialData, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_MaterialUBO); // binding = 1 matches shader
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		static ET_LIGHTLUTID ids[] = { ET_LUT_D0, ET_LUT_D1, ET_LUT_FR, ET_LUT_RB, ET_LUT_RG, ET_LUT_RR };

		for (int i = 0; i < 6; i++)
		{
			m_LutConfigs[i].id = ids[i];
		}

		glGenBuffers(1, &m_TexEnvUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, m_TexEnvUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOTexEnvData), &m_TexEnvData, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_TexEnvUBO); // binding = 2 matches shader
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
	}

	OpenGLLightEnvironment::~OpenGLLightEnvironment()
	{
		if (m_LightEnvUBO)
			glDeleteBuffers(1, &m_LightEnvUBO);

		if (m_MaterialUBO)
			glDeleteBuffers(1, &m_MaterialUBO);

		if (m_TexEnvUBO)
			glDeleteBuffers(1, &m_TexEnvUBO);
	}

	void OpenGLLightEnvironment::Bind()
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_LightEnvUBO);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_MaterialUBO);
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_TexEnvUBO);
	}

	void OpenGLLightEnvironment::BindMaterial(Material* material)
	{
		// Bind Material Values
		MaterialValues& values   = material->GetProps().Values;
		m_MaterialData.ambient   = { values.Ambient[0],   values.Ambient[1],   values.Ambient[2],   1.0f };
		m_MaterialData.diffuse   = { values.Diffuse[0],   values.Diffuse[1],   values.Diffuse[2],   1.0f };
		m_MaterialData.specular0 = { values.Specular0[0], values.Specular0[1], values.Specular0[2], 1.0f };
		m_MaterialData.specular1 = { values.Specular1[0], values.Specular1[1], values.Specular1[2], 1.0f };
		m_MaterialData.emissive  = { values.Emission[0],  values.Emission[1],  values.Emission[2],  1.0f };

		glBindBuffer(GL_UNIFORM_BUFFER, m_MaterialUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UBOMaterialData), &m_MaterialData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Bind TexEnvs
		int texEnvCount = material->TexEnvCount();
		UBOTexEnvData uboTexEnvData;
		for (int i = 0; i < texEnvCount; i++)
		{
			const TexEnvProps& tep = material->GetTexEnvProps(i);

			uboTexEnvData.texEnvConfig[i].x = tep.Channels;

			uboTexEnvData.texEnvRGBInputs[i].x = tep.BlendMode;
			uboTexEnvData.texEnvRGBInputs[i].y = tep.Source1;
			uboTexEnvData.texEnvRGBInputs[i].z = tep.Source2;
			uboTexEnvData.texEnvRGBInputs[i].w = tep.Source3;
			
			uboTexEnvData.texEnvAlphaInputs[i].x = tep.AlphaBlendMode;
			uboTexEnvData.texEnvAlphaInputs[i].y = tep.AlphaSource1;
			uboTexEnvData.texEnvAlphaInputs[i].z = tep.AlphaSource2;
			uboTexEnvData.texEnvAlphaInputs[i].w = tep.AlphaSource3;
		}

		glBindBuffer(GL_UNIFORM_BUFFER, m_TexEnvUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UBOTexEnvData), &uboTexEnvData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	int OpenGLLightEnvironment::LightInit(Ref<Light> light)
	{
		int id;
		for (id = 0; id < MAX_LIGHTS; id++)
			if (!m_HasLight[id])
				break;
		if (id == 8) return -1;

		m_HasLight[id] = true;

		light->SetID(id);
		light->SetParent(m_LightEnvUBO);

		m_LightCount++;
		return id;
	}
	void OpenGLLightEnvironment::LightDestroy(Ref<Light> light)
	{
		if (light->GetID() == -1) return;

		m_HasLight[light->GetID()] = false;
		m_LightCount--;

		OGL_Light empty = {};

		glBindBuffer(GL_UNIFORM_BUFFER, m_LightEnvUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(OGL_Light) * light->GetID(), sizeof(OGL_Light), &empty);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLLightEnvironment::SetSceneAmbientColor(glm::vec3 color)
	{
		m_SceneAmbient = color;
		glBindBuffer(GL_UNIFORM_BUFFER, m_LightEnvUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(OGL_Light) * MAX_LIGHTS, sizeof(glm::vec3), &color);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLLightEnvironment::ConfigureLut(LutConfig& config)
	{
		int ids[] = { 0, 1, -1, 2, 3, 4, 5,-1 };
		int id = ids[config.id];
		if (id >= 0)
		{
			LightLut lut;

			switch (config.funcType)
			{
			case LutFuncType::Pow:
				ET_LightLut_Phong(&lut, config.funcArgs.powExponent);
				break;
			case LutFuncType::Spotlight:
				ET_LightLut_Spotlight(&lut, config.funcArgs.spotlightCutoff);
				break;
			case LutFuncType::ToonDiffuse:
				ET_LightLut_ToonDiffuse(&lut, 0.0f);
				break;
			case LutFuncType::ToonSpecular:
				ET_LightLut_ToonDiffuse(&lut, config.funcArgs.toonShininess);
				break;
			case LutFuncType::Custom:
				break;
			default:
				ET_LightLut_Zeroes(&lut, 0.0f);
			break;
			}

			m_Luts[id] = lut;

			// Prepare for Shader
			glm::ivec4 lutParams;
			glm::ivec4 uboLut[64]; // 256 ints
			
			m_LutConfigs[id] = config;
			lutParams.x = config.input;
			
			IntArrayToIvec4Array(&lut.data[0], &uboLut[0]);
			size_t lutSize = sizeof(glm::ivec4) * 64;

			// Upload
			glBindBuffer(GL_UNIFORM_BUFFER, m_LightEnvUBO);
			glBufferSubData(GL_UNIFORM_BUFFER, (sizeof(OGL_Light) * MAX_LIGHTS) + sizeof(glm::vec4) + (sizeof(glm::ivec4) * id), sizeof(glm::ivec4), &lutParams);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glBindBuffer(GL_UNIFORM_BUFFER, m_LightEnvUBO);
			glBufferSubData(GL_UNIFORM_BUFFER, (sizeof(OGL_Light) * MAX_LIGHTS) + sizeof(glm::vec4) + (sizeof(glm::ivec4) * MAX_LUTS) + (lutSize * id), lutSize, &uboLut);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}
}
