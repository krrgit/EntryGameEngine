#pragma once

#include "Entry/Renderer/Light.h"

#include <glad/glad.h>

namespace Entry
{
	struct OGL_Light
	{
		glm::vec4 position;
		glm::vec4 direction;
		glm::vec4 color;
		glm::vec4 params;
		glm::ivec4 daLut[64];
		glm::ivec4 spLut[64];
	};

	class OpenGLLight : public Light
	{
	public:
		OpenGLLight(LightProps& props);
		virtual ~OpenGLLight();

		virtual void SetLight(LightProps props) override;

		virtual void SetID(int id) override { m_LightID = id; }
		virtual int GetID() override { return m_LightID; }

		virtual void* GetNativeLight() override { return (void*)m_LightID; }

		virtual void SetParent(uint32_t parent) override { m_Parent = parent; }

		virtual void SetAsDirectionalLight() override;
		virtual void SetAsPointLight(float linear, float quad) override;
		virtual void SetAsSpotLight(float angle) override;

		virtual void SetupLight() override;
		
	private:
		int m_LightID = -1;
		GLint m_Parent;
		OGL_Light m_LightData;
		float m_PositionalLight = 0.0f; // Really, is just a bool

	};
}