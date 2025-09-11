#pragma once

#include "Entry/Renderer/Material.h"

namespace Entry {
    class OpenGLMaterial : public Material
    {
    public:
        OpenGLMaterial() = default;
        ~OpenGLMaterial() = default;
        OpenGLMaterial(const MaterialProps& props);

        virtual void Bind() override;
        virtual MaterialProps& GetProps() { return m_Props; }
        virtual Shader* GetShader() override { return m_Props.ShaderProgram; }

    private:
        MaterialProps m_Props;
    };
}
