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
        virtual ShaderProgram GetShader() override { return m_Props.shader; }
        virtual void SetShader(ShaderProgram program) { m_Props.shader = program; OnShaderChange(); };

        virtual void SetTexEnvProps(TexEnvProps& props) { m_TexEnvProps = props; }
        virtual const TexEnvProps& GetTexEnvProps() { return m_TexEnvProps; }
    private:
        void OnShaderChange();
    private:
        MaterialProps m_Props;
        TexEnvProps m_TexEnvProps;
    };
}
