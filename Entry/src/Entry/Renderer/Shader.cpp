#include "etpch.h"
#include "Shader.h"


#include "vshader00_shbin.h"
#include "vshader01_shbin.h"

namespace Entry {

Shader::Shader(int src_id)
{
    // TODO: switch to runtime compilation or make a better LUT
    switch (src_id) {
    case 0:
        vshader_dvlb = DVLB_ParseFile((u32*)vshader00_shbin, vshader00_shbin_size);
        break;
    case 1:
        vshader_dvlb = DVLB_ParseFile((u32*)vshader01_shbin, vshader01_shbin_size);
        break;
    default:
        ET_CORE_ASSERT(false, "Shader ID out of bounds. Using ID 0.");
        vshader_dvlb = DVLB_ParseFile((u32*)vshader00_shbin, vshader00_shbin_size);
        break;
    }

    shaderProgramInit(&program);
    shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
}

Shader::~Shader()
{
    shaderProgramFree(&program);
    DVLB_Free(vshader_dvlb);
}

void Shader::Bind()
{
    C3D_BindProgram(&program);
}

void Shader::Unbind() const
{
    C3D_BindProgram(0);
}

}