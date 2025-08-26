#pragma once

#include "Entry/Core/Core.h"

#include "Texture.h"

namespace Entry {
	struct MaterialProps 
	{
		float Ambient[3];
		float Diffuse[3];
		float Specular0[3];
		float Specular1[3];
		float Emission[3];

		std::string Name;
		Ref<Texture2D> DiffuseMap;
	};

	class Material {
	public:
		virtual ~Material() = default;

		virtual void Bind() = 0;
		virtual const MaterialProps& GetProps() const = 0;

		static Ref<Material> Create(MaterialProps& props);
	};
}