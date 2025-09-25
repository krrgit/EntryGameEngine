#pragma once

#include "Entry/Core/Core.h"
#include "Shader.h"
#include "Texture.h"
#include "TextureEnvironment.h"

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
		ShaderProgram shader;
		//bool ColorValues = true;
	};

	class Material {
	public:
		virtual ~Material() = default;

		virtual void Bind() = 0;
		virtual MaterialProps& GetProps() = 0;
		virtual ShaderProgram GetShader() = 0;
		virtual void SetShader(ShaderProgram program) = 0;

		virtual void SetTexEnvProps(TexEnvProps& props, int id = 0) = 0;
		virtual const TexEnvProps& GetTexEnvProps(int id = 0) = 0;
		virtual const int TexEnvCount() = 0;

		static Ref<Material> Create(const MaterialProps& props);
	};
}