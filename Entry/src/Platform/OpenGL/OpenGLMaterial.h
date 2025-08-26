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

    private:
        MaterialProps m_Props;
    };
}
