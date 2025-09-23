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
        virtual void SetShader(ShaderProgram program) override { m_Props.shader = program; OnShaderChange(); };

        virtual void SetTexEnvProps(TexEnvProps& props, int id = 0) override;
        virtual const TexEnvProps& GetTexEnvProps(int id = 0) override { return m_TexEnvProps[id]; }
    private:
        void OnShaderChange();
    private:
        MaterialProps m_Props;
        TexEnvProps m_TexEnvProps[6];
    };
}
