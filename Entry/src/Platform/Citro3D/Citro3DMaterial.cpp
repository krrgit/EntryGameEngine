#include "etpch.h"
#include "Citro3DMaterial.h"

namespace Entry {
	Citro3DMaterial::Citro3DMaterial(const MaterialProps& props)
		: m_Props(props)
	{
		OnShaderChange();
	}

	void Citro3DMaterial::Bind()
	{
		SetTexEnvs();
		m_Props.DiffuseMap->Bind();
	}

	void Citro3DMaterial::SetTexEnvs() 
	{
		// TODO: add support to set multiple channels
		C3D_TexEnv* env = C3D_GetTexEnv(0);

		C3D_TexEnvSrc(
			env, 
			(C3D_TexEnvMode) m_TexEnvProps.Channels, 
			(GPU_TEVSRC) m_TexEnvProps.Source1, 
			(GPU_TEVSRC)m_TexEnvProps.Source2,
			(GPU_TEVSRC)m_TexEnvProps.Source3
		);
		C3D_TexEnvFunc(
			env, 
			(C3D_TexEnvMode)m_TexEnvProps.Channels,
			(GPU_COMBINEFUNC)m_TexEnvProps.BlendMode
		);
	}

	void Citro3DMaterial::OnShaderChange()
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