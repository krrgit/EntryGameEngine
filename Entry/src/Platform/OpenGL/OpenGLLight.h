#pragma once

#include "Entry/Renderer/Light.h"

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

	struct UBOLightData
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

	class OpenGLLight : public Light
	{
	public:
		OpenGLLight(LightProps& props);
		virtual ~OpenGLLight();

		virtual void SetLight(LightProps props) override;
	private:
		uint16_t m_LightID;
		UBOLightData m_LightData;
		
		UBOMaterialData m_MaterialData;
	};
}