#include "etpch.h"
#include "OpenGLLight.h"
#include "Entry/Renderer/LightLut.h"

namespace Entry
{

	OpenGLLight::OpenGLLight(LightProps& props)
	{ 
		m_PositionalLight = props.Type == ET_DirectionalLight ? 0.0f : 1.0;
		m_LightData.position = glm::vec4(props.Position, m_PositionalLight);
		m_LightData.color = glm::vec4(props.Color, props.Strength);
		m_LightData.params.x = (float)props.Type;
		//m_LightData.params.y = props.Strength;
		m_LightData.params.z = props.Angle;

		switch (props.Type)
		{
		case ET_DirectionalLight:
		break;
		case ET_PointLight:
		break;
		case ET_Spotlight:
			SetAsSpotLight(props.Angle);
		break;
		default:
		break;
		}
	}

	OpenGLLight::~OpenGLLight()
	{
	}

	void OpenGLLight::SetLight(LightProps props)
	{
		m_PositionalLight = props.Type == ET_DirectionalLight ? 0.0f : 1.0f;
		m_LightData.position  = glm::vec4(props.Position, m_PositionalLight);
		m_LightData.direction = glm::vec4(props.Direction, 1.0f);
		m_LightData.color     = glm::vec4(props.Color, props.Strength);
		m_LightData.params.x = (float)props.Type;
		//m_LightData.params.y = props.Strength;
		m_LightData.params.z = props.Angle;


		glBindBuffer(GL_UNIFORM_BUFFER, m_Parent);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(OGL_Light) * m_LightID, sizeof(OGL_Light), &m_LightData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLLight::SetAsDirectionalLight()
	{
	}

	void OpenGLLight::SetAsPointLight(float linear, float quad)
	{
		// Set DA Lut
		LightLutDA daLut;
		ET_LightLutDA_Quadratic(&daLut, 0.0f, 75.0f, linear, quad);
		IntArrayToIvec4Array(&daLut.lut.data[0], &m_LightData.daLut[0]);
	}

	void OpenGLLight::SetAsSpotLight(float angle)
	{
		// Set SP Lut
		LightLut spLut;
		ET_LightLut_Spotlight(&spLut, (180.0f - angle)/ 90.0f);
		IntArrayToIvec4Array(&spLut.data[0], &m_LightData.spLut[0]);
	}

	void OpenGLLight::SetupLight()
	{
	}
}