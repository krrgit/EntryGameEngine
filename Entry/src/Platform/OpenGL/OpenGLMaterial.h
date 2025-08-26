#pragma once

#include "Entry/Renderer/Material.h"

namespace Entry {
    class OpenGLMaterial : public Material
    {
    public:
        OpenGLMaterial() = default;
        OpenGLMaterial(MaterialProps& props);

        virtual void Bind() override;
        virtual const MaterialProps& GetProps() const { return m_Props; }

        ~OpenGLMaterial();
    private:
        MaterialProps m_Props;
    };
}
