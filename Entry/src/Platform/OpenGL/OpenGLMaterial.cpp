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
		BindTexEnvs();
		if (m_Props.DiffuseMap) m_Props.DiffuseMap->Bind();
	}

	void OpenGLMaterial::BindTexEnvs()
	{
		glm::ivec4 texEnvsData[6];
		// Set Used TexEnv
		int i = 0;
		for (; i < m_TexEnvCount; i++)
		{
			texEnvsData[i].x = m_TexEnvProps[i].BlendMode;
			texEnvsData[i].y = m_TexEnvProps[i].Source1;
			texEnvsData[i].z = m_TexEnvProps[i].Source2;
			texEnvsData[i].w = m_TexEnvProps[i].Source3;
		}

		// Reset Unused TexEnvs
		for (; i < 6; i++)
		{
			texEnvsData[i].x = TexEnvBlendMode::ET_GPU_REPLACE;
			texEnvsData[i].y = TexEnvSource::ET_GPU_PREVIOUS;
			texEnvsData[i].z = TexEnvSource::ET_GPU_PRIMARY_COLOR;
			texEnvsData[i].w = TexEnvSource::ET_GPU_PRIMARY_COLOR;
		}

		// Upload

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
			m_TexEnvCount = 2;
			break;
		case ShaderProgramEnum::Unlit:
			for (int i = 0; i < 6; i++)
				m_TexEnvProps[i] = s_TexEnvUnlitProps[i];
			m_TexEnvCount = 1;
			break;
		default:
		break;
		}
	}
}