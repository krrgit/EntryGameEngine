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
		//BindTexEnvs();
		m_Props.DiffuseMap->Bind();
	}

	//void Citro3DMaterial::BindTexEnvs()
	//{
	//	// Set Used TexEnv
	//	int i = 0;
	//	for (; i < m_TexEnvCount; i++)
	//	{
	//		SetTexEnv(i);
	//	}

	//	// Reset Unused TexEnvs
	//	for (; i < 6; i++)
	//	{
	//		C3D_TexEnvInit(C3D_GetTexEnv(i));
	//	}
	//}

	void Citro3DMaterial::SetTexEnvProps(TexEnvProps& props, int id)
	{
		m_TexEnvProps[id] = props;
	}

	void Citro3DMaterial::OnShaderChange()
	{
		switch (m_Props.shader)
		{
		case ShaderProgramEnum::Lit:
			for (int i = 0; i < 6; i++)
				m_TexEnvProps[i] = s_TexEnvLitProps[i];

			m_TexEnvCount = 1;
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