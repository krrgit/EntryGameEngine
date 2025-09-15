#pragma once

#include "Entry/Renderer/Light.h"

#include <glad/glad.h>

namespace Entry
{
	struct UBOLightData
	{
		glm::vec4 position;
		glm::vec4 color;
		glm::vec4 params;
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
		GLuint m_LightUBO;
		UBOLightData m_LightData;

		GLuint m_MaterialUBO;
		UBOMaterialData m_MaterialData;
	};
}