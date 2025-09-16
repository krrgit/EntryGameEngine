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
	}

	void OpenGLLightEnvironment::Bind()
	{
		// Do nothing
	}

	void OpenGLLightEnvironment::SetMaterial(Ref<Material> material)
	{
		MaterialValues& values   = material->GetProps().Values;
		m_MaterialData.ambient   = { values.Ambient[0],   values.Ambient[1],   values.Ambient[2],   1.0f };
		m_MaterialData.diffuse   = { values.Diffuse[0],   values.Diffuse[1],   values.Diffuse[2],   1.0f };
		m_MaterialData.specular0 = { values.Specular0[0], values.Specular0[1], values.Specular0[2], 1.0f };
		m_MaterialData.specular1 = { values.Specular1[0], values.Specular1[1], values.Specular1[2], 1.0f };
		m_MaterialData.emissive  = { values.Emission[0],  values.Emission[1],  values.Emission[2],  1.0f };

		glBindBuffer(GL_UNIFORM_BUFFER, m_MaterialUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UBOMaterialData), &m_MaterialData);
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
}
