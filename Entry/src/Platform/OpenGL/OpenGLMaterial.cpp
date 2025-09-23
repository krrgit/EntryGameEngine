#include "etpch.h"
#include "OpenGLMaterial.h"

namespace Entry {
	OpenGLMaterial::OpenGLMaterial(const MaterialProps& props)
		: m_Props(props)
	{
		OnShaderChange();
	}

	void OpenGLMaterial::Bind()
	{
		m_Props.DiffuseMap->Bind();
	}
	void OpenGLMaterial::SetTexEnvProps(TexEnvProps& props, int id)
	{
		m_TexEnvProps[id] = props;
	}
	void OpenGLMaterial::OnShaderChange()
	{
		switch (m_Props.shader)
		{
		case ShaderProgramEnum::Lit:
			for(int i=0;i<6;i++)
				m_TexEnvProps[i] = s_TexEnvLitProps[i];
			break;
		case ShaderProgramEnum::Unlit:
			for (int i = 0; i < 6; i++)
				m_TexEnvProps[i] = s_TexEnvUnlitProps[i];
			break;
		default:
		break;
		}
	}
}