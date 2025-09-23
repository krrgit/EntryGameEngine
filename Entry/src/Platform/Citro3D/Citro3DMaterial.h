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

        virtual void SetTexEnvProps(TexEnvProps& props, int id = 0) override;
        virtual const TexEnvProps& GetTexEnvProps(int id = 0) { return m_TexEnvProps[id]; }
    private:
        void OnShaderChange();
        void SetTexEnvs();
        void SetTexEnv(int id = 0);

    private:
        MaterialProps m_Props;
        TexEnvProps m_TexEnvProps[6];
        int m_TexEnvSize = 0;
    };
}
