#pragma once

#include <citro3d.h>
#include "Entry/Renderer/Material.h"

namespace Entry {
	class Citro3DMaterial : public Material
	{
	public:
		Citro3DMaterial(MaterialProps& props);
		~Citro3DMaterial();

		virtual void Bind() override;
		virtual const MaterialProps& GetProps() const { return m_Props; }

	private:
		MaterialProps m_Props;
		C3D_Material m_Material;
	};

}
