#include "etpch.h"
#include "Citro3DMaterial.h"

namespace Entry {
	Citro3DMaterial::Citro3DMaterial(MaterialProps& props)
		: m_Props(props)
	{
		m_Material = {
			props.Ambient[0],   props.Ambient[1],   props.Ambient[2],
			props.Diffuse[0],   props.Diffuse[1],   props.Diffuse[2],
			props.Specular0[0],	props.Specular0[1],	props.Specular0[2],
			props.Specular1[0],	props.Specular1[1],	props.Specular1[2],
			props.Emission[0],	props.Emission[1],	props.Emission[2]
		};

	}
	
    Citro3DMaterial::~Citro3DMaterial()
    {
    }

	void Citro3DMaterial::Bind()
	{
		m_Props.DiffuseMap->Bind(0);
	}
}