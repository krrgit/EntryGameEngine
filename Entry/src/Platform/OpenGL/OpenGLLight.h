#pragma once

#include "Entry/Renderer/Light.h"

#include <glad/glad.h>



namespace Entry
{
	class LightEnvironment;

	struct OGL_Light
	{
		glm::vec4 position;
		glm::vec4 color;
		glm::vec4 params;
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

		virtual void SetParent(int parent) override { m_Parent = parent; }
		
	private:
		int m_LightID = -1;
		GLint m_Parent;
		OGL_Light m_LightData;
	};
}