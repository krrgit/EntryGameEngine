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

    private:
        MaterialProps m_Props;
    };
}
