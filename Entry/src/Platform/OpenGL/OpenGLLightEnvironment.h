#pragma once

#include "Entry/Renderer/LightEnvironment.h"

#include <glad/glad.h>

// 3DS Max Light Limit
#define MAX_LIGHTS 8

namespace Entry
{
	struct OGL_Light
	{
		glm::vec4 position;
		glm::vec4 color;
		glm::vec4 params;
	};

	struct UBOLightEnvData
	{
		OGL_Light lights[MAX_LIGHTS];
		glm::vec4 sceneAmbient;
	};

	struct UBOMaterialData
	{
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular0;
		glm::vec4 specular1;
		glm::vec4 emissive;
	};

	class OpenGLLightEnvironment : public LightEnvironment
	{
	public:
		OpenGLLightEnvironment();
		~OpenGLLightEnvironment() = default;
		
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
		virtual glm::vec3 GetSceneAmbientColor() override { return m_SceneAmbient; }


	private:
		GLuint m_LightEnvUBO;

		GLuint m_MaterialUBO;
		UBOMaterialData m_MaterialData;

		glm::vec3 m_SceneAmbient;
		Ref<Material> m_Material;

		OGL_Light* m_Lights[MAX_LIGHTS];

		uint16_t m_LightCount = 0;
		bool m_HasLight[MAX_LIGHTS] = {0,0,0,0,0,0,0,0};
	};
}