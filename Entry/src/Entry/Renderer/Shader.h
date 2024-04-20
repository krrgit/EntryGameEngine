#pragma once

#include <citro3d.h>

namespace Entry {
	class Shader {
	public:
		Shader(int src_id);
		~Shader();

		void Bind();
		void Unbind() const;
	private:
		DVLB_s* vshader_dvlb;
		shaderProgram_s program;
	};
}