#include "etpch.h"
#include "OpenGLMaterial.h"

namespace Entry {

    OpenGLMaterial::OpenGLMaterial(MaterialProps& props)
        : m_Props(props)
    {
    }

    void OpenGLMaterial::Bind()
    {
        m_Props.DiffuseMap->Bind();
    }

    OpenGLMaterial::~OpenGLMaterial()
    {
    }
}