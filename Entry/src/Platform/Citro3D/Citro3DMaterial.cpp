#include "etpch.h"
#include "Citro3DMaterial.h"

namespace Entry {
	Citro3DMaterial::Citro3DMaterial(const MaterialProps& props)
		: m_Props(props)
	{
	}

	void Citro3DMaterial::Bind()
	{
		m_Props.DiffuseMap->Bind();
	}
}