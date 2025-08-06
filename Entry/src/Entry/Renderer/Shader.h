#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Entry {
	class Shader {
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		static Shader* Create(const uint8_t* shbinData, u32 shBinSize);
	};
}