#pragma once


namespace Entry {
	class Shader {
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() const = 0;
		static Shader* Create(int src_id);
	};
}