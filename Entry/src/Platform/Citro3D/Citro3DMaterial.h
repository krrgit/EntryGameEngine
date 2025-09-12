#pragma once

#include "Entry/Renderer/Material.h"

namespace Entry {

    class Citro3DMaterial : public Material
    {
    public:
        Citro3DMaterial() = default;
        ~Citro3DMaterial() = default;
        Citro3DMaterial(const MaterialProps& props);

        virtual void Bind() override;
        virtual MaterialProps& GetProps() { return m_Props; }
        virtual ShaderProgram GetShader() override { return m_Props.shader; }
        virtual void SetShader(ShaderProgram program) { m_Props.shader = program; OnShaderChange(); }

        virtual void SetTexEnvProps(TexEnvProps& props) { m_TexEnvProps = props; }
        virtual const TexEnvProps& GetTexEnvProps() { return m_TexEnvProps; }
    private:
        void OnShaderChange();
        void SetTexEnvs();
    private:
        MaterialProps m_Props;
        TexEnvProps m_TexEnvProps;
    };
}
