#pragma once

#include "Entry/Core/Core.h"

#include "Texture.h"

namespace Entry {
	struct MaterialValues {
		float Ambient[3];
		float Diffuse[3];
		float Specular0[3];
		float Specular1[3];
		float Emission[3];
	};

	struct MaterialProps 
	{
		MaterialValues Values;
		std::string Name;
		Ref<Texture2D> DiffuseMap;
	};

	class Material {
	public:
		virtual ~Material() = default;

		virtual void Bind() = 0;
		virtual MaterialProps& GetProps() = 0;

		static Ref<Material> Create(const MaterialProps& props);
	};
}