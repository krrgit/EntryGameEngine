#pragma once

#include "Entry/Renderer/Shader.h"

#include <citro3d.h>
#include <glm/glm.hpp>

namespace Entry {
	class Citro3DShader : public Shader {
	public:
		Citro3DShader(u32* shbinData, u32 shBinSize);
		~Citro3DShader();

		virtual void Bind();
		virtual void Unbind() const;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override {}
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		void UploadUniformInt(std::string name, int value);
		void UploadUniformFloat(std::string name, const float value);
		void UploadUniformFloat2(std::string name, const glm::vec2& value);
		void UploadUniformFloat3(std::string name, const glm::vec3& value);
		void UploadUniformFloat4(std::string name, const glm::vec4& value);

		void UploadUniformMat3(std::string name, const glm::mat3* matrix);
		void UploadUniformMat4(std::string name, const glm::mat4& matrix);
	private:
		uint32_t m_RendererID;
		DVLB_s* vshader_dvlb;
		shaderProgram_s program;
	};
}