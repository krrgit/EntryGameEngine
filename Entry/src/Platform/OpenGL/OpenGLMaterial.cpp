#include "etpch.h"
#include "OpenGLMaterial.h"

namespace Entry {
	OpenGLMaterial::OpenGLMaterial(const MaterialProps& props)
		: m_Props(props)
	{
	}

	void OpenGLMaterial::Bind()
	{
		m_Props.DiffuseMap->Bind();
	}
	void OpenGLMaterial::OnShaderChange()
	{
		switch (m_Props.shader)
		{
		case ShaderProgramEnum::Lit:
			m_TexEnvProps = s_TexEnvLitProps;
			break;
		case ShaderProgramEnum::Unlit:
			m_TexEnvProps = s_TexEnvUnlitProps;
			break;
		default:
		break;
		}
	}
}