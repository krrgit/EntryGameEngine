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
		// Set Used TexEnv
		int i = 0;
		for (; i < m_TexEnvSize; i++)
		{
			SetTexEnv(i);
		}

		// Reset Unused TexEnvs
		for (; i < 6; i++)
		{
			C3D_TexEnvInit(C3D_GetTexEnv(i));
		}
	}

	void Citro3DMaterial::SetTexEnv(int id) 
	{
		C3D_TexEnv* env = C3D_GetTexEnv(id);

		TexEnvProps& texEnvProps = m_TexEnvProps[id];

		switch (texEnvProps.Channels)
		{
		case TexEnvChannels::ET_RGBA:
		{
			C3D_TexEnvSrc(
				env,
				(C3D_TexEnvMode)texEnvProps.Channels,
				(GPU_TEVSRC)texEnvProps.Source1,
				(GPU_TEVSRC)texEnvProps.Source2,
				(GPU_TEVSRC)texEnvProps.Source3
			);
			C3D_TexEnvFunc(
				env,
				(C3D_TexEnvMode)texEnvProps.Channels,
				(GPU_COMBINEFUNC)texEnvProps.BlendMode
			);
		}
		break;
		case TexEnvChannels::ET_Alpha:
		{
			// Reset RGB
			C3D_TexEnvSrc(
				env,
				C3D_RGB,
				GPU_PREVIOUS,
				GPU_PRIMARY_COLOR,
				GPU_PRIMARY_COLOR
			);
			C3D_TexEnvFunc(
				env,
				C3D_RGB,
				GPU_REPLACE
			);

			// Set Alpha
			C3D_TexEnvSrc(
				env,
				C3D_Alpha,
				(GPU_TEVSRC)texEnvProps.AlphaSource1,
				(GPU_TEVSRC)texEnvProps.AlphaSource2,
				(GPU_TEVSRC)texEnvProps.AlphaSource3
			);
			C3D_TexEnvFunc(
				env,
				C3D_Alpha,
				(GPU_COMBINEFUNC)texEnvProps.AlphaBlendMode
			);
		}
		break;
		case TexEnvChannels::ET_RGB:
		{
			// Set RGB
			C3D_TexEnvSrc(
				env,
				C3D_RGB,
				(GPU_TEVSRC)texEnvProps.Source1,
				(GPU_TEVSRC)texEnvProps.Source2,
				(GPU_TEVSRC)texEnvProps.Source3
			);
			C3D_TexEnvFunc(
				env,
				C3D_RGB,
				(GPU_COMBINEFUNC)texEnvProps.BlendMode
			);

			// Reset Alpha
			C3D_TexEnvSrc(
				env,
				C3D_Alpha,
				GPU_PREVIOUS,
				GPU_PRIMARY_COLOR,
				GPU_PRIMARY_COLOR
			);
			C3D_TexEnvFunc(
				env,
				C3D_Alpha,
				GPU_REPLACE
			);
		}
		break;
		case TexEnvChannels::ET_RGBA_Separate:
		{
			// Set RGB
			C3D_TexEnvSrc(
				env,
				C3D_RGB,
				(GPU_TEVSRC)texEnvProps.Source1,
				(GPU_TEVSRC)texEnvProps.Source2,
				(GPU_TEVSRC)texEnvProps.Source3
			);
			C3D_TexEnvFunc(
				env,
				C3D_RGB,
				(GPU_COMBINEFUNC)texEnvProps.BlendMode
			);

			// Set Alpha
			C3D_TexEnvSrc(
				env,
				C3D_Alpha,
				(GPU_TEVSRC)texEnvProps.AlphaSource1,
				(GPU_TEVSRC)texEnvProps.AlphaSource2,
				(GPU_TEVSRC)texEnvProps.AlphaSource3
			);
			C3D_TexEnvFunc(
				env,
				C3D_Alpha,
				(GPU_COMBINEFUNC)texEnvProps.AlphaBlendMode
			);
		}
		break;
		default:
		break;
		}
	}


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

			m_TexEnvSize = 2;
			break;
		case ShaderProgramEnum::Unlit:
			for (int i = 0; i < 6; i++)
				m_TexEnvProps[i] = s_TexEnvUnlitProps[i];

			m_TexEnvSize = 1;
			break;
		default:
			break;
		}
	}
}