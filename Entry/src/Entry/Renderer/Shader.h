#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Entry {

	enum ShaderProgramEnum : int
	{
		Lit,
		Unlit,
		// Toon,		// TODO
		// VertexLit,	// TODO 
		NoShader = -1,
	};
	struct ShaderProgram
	{
		ShaderProgramEnum value = ShaderProgramEnum::Lit;

		ShaderProgram() = default;

		ShaderProgram(ShaderProgramEnum v)
			: value(v) 
		{}

		std::string string() const
		{
			switch (value)
			{
			case Lit:   return "Lit";
			case Unlit: return "Unlit";
				// case Toon: return "Toon";
				// case VertexLit: return "VertexLit";
			default:    return "Unknown";
			}
		}

		// implicit conversion back to enum
		operator ShaderProgramEnum() const { return value; }
	};

	class Shader {
	public:
		virtual ~Shader() = default;

		virtual std::string GetName() const = 0;

		virtual void Bind() = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		static Shader* Create(const uint8_t* shbinData, uint32_t shBinSize);
		static Shader* Create(const std::string& filepath);
	};
}