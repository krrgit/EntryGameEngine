#include "etpch.h"
#include "OpenGLLight.h"

namespace Entry
{
	static float s_LightCount = 0;
	static GLuint s_LightUBO;
	static GLuint s_MaterialUBO;

	OpenGLLight::OpenGLLight(LightProps& props)
	{
		m_LightID = (int)s_LightCount++;
		m_LightData.lights[m_LightID] =
		{
			{0,0,0,0},
			{props.Color.r, props.Color.g, props.Color.b, 1.0f},
			{props.Strength, props.Angle, 0, 0},
		};


		if (m_LightID == 0)
		{
			m_LightData.sceneAmbient = glm::vec4(0);
	
			glGenBuffers(1, &s_LightUBO);
			glBindBuffer(GL_UNIFORM_BUFFER, s_LightUBO);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOLightData), &m_LightData, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, s_LightUBO); // binding = 0 matches shader
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			m_MaterialData =
			{
				{ 0.2f, 0.2f, 0.2f, 0.0f }, //ambient
				{ 0.4f, 0.4f, 0.4f, 0.0f }, //diffuse
				{ 0.8f, 0.8f, 0.8f, 0.0f }, //specular0
				{ 0.0f, 0.0f, 0.0f, 0.0f }, //specular1
				{ 0.0f, 0.0f, 0.0f, 0.0f }, //emission
			};

			glGenBuffers(1, &s_MaterialUBO);
			glBindBuffer(GL_UNIFORM_BUFFER, s_MaterialUBO);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOMaterialData), &m_MaterialData, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, 1, s_MaterialUBO); // binding = 1 matches shader
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}


	}

	OpenGLLight::~OpenGLLight()
	{
		m_LightData.lights[m_LightID] = OGL_Light{};
		s_LightCount -= 1.0f;

		if (s_LightCount < 1.0f)
		{
			s_LightCount = 0;
			ET_CORE_ERROR("Destroy Light UBO");
			glDeleteBuffers(sizeof(UBOLightData), &s_LightUBO);
			glDeleteBuffers(sizeof(UBOMaterialData), &s_MaterialUBO);
		}
	}

	void OpenGLLight::SetLight(LightProps props)
	{
		m_LightData.lights[m_LightID].position = glm::vec4(props.Position, 1.0f);
		m_LightData.lights[m_LightID].color = glm::vec4(props.Color, 1.0f);
		m_LightData.lights[m_LightID].params.x = props.Strength;
		m_LightData.lights[m_LightID].params.y = props.Angle;

		glBindBuffer(GL_UNIFORM_BUFFER, s_LightUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(OGL_Light) * m_LightID, sizeof(OGL_Light), &m_LightData.lights[m_LightID]);
		glBufferSubData(GL_UNIFORM_BUFFER, (sizeof(OGL_Light) * MAX_LIGHTS) + sizeof(glm::vec3), sizeof(float), &s_LightCount);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}