#pragma once

#include "Entry/Renderer/LightEnvironment.h"
#include "Platform/OpenGL/OpenGLLight.h"

#include <glad/glad.h>

namespace Entry
{
	struct UBOLightEnvData
	{
		OGL_Light lights[MAX_LIGHTS];
		glm::vec4 sceneAmbient;
		glm::ivec4 params[MAX_LUTS]; // See shader for details
		glm::ivec4 luts[MAX_LUTS * 64];
	};


	struct UBOMaterialData
	{
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular0;
		glm::vec4 specular1;
		glm::vec4 emissive;
	};

	struct UBOTexEnvData
	{
		glm::ivec4 texEnvConfig[MAX_TEXENV];
		glm::ivec4 texEnvRGBInputs[MAX_TEXENV];
		glm::ivec4 texEnvAlphaInputs[MAX_TEXENV];
	};

	class OpenGLLightEnvironment : public LightEnvironment
	{
	public:
		OpenGLLightEnvironment();
		~OpenGLLightEnvironment();
		
		virtual void Bind() override;
		virtual void BindMaterial(Material* material) override;

		virtual int LightInit(Ref<Light> light) override;
		virtual void LightDestroy(Ref<Light> light) override;

		virtual int GetLightCount() override { return m_LightCount; }

		virtual void SetSceneAmbientColor(glm::vec3 color) override;
		virtual glm::vec3 GetSceneAmbientColor() override { return m_SceneAmbient; }

		virtual void ConfigureLut(LutConfig& config) override;
		virtual LutConfig& GetLutConfig(ET_LIGHTLUTID id) override 
		{
			static int ids[] = { 0, 1, -1, 2, 3, 4, 5,-1 };
			return m_LutConfigs[ids[id]];
		}

	private:

	private:
		GLuint m_LightEnvUBO;

		GLuint m_MaterialUBO;
		UBOMaterialData m_MaterialData;

		GLuint m_TexEnvUBO;
		UBOTexEnvData m_TexEnvData;

		OGL_Light* m_Lights[MAX_LIGHTS];

		glm::vec3 m_SceneAmbient;

		uint16_t m_LightCount = 0;
		bool m_HasLight[MAX_LIGHTS] = {0,0,0,0,0,0,0,0};

		LightLut m_Luts[6];
		LutConfig m_LutConfigs[6];
	};
}