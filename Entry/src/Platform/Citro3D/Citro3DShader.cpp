#include "etpch.h"
#include "Citro3DShader.h"

namespace Entry {
    static uint32_t s_NextRendererID = 0;
    void glm_mat3_to_C3D_mtx(C3D_Mtx* out, const glm::mat3* in) {
        Mtx_Zeros(out);
        for(int i=0;i<3;++i){
            out->r[i].x = (*in)[i][0];
            out->r[i].y = (*in)[i][1];
            out->r[i].z = (*in)[i][2];
        }
    }

    Citro3DShader::Citro3DShader(u32* shbinData, u32 shBinSize)
    {
        vshader_dvlb = DVLB_ParseFile(shbinData, shBinSize);
        m_RendererID = s_NextRendererID++;
        shaderProgramInit(&program);
        shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
    }

    Citro3DShader::~Citro3DShader()
    {
        shaderProgramFree(&program);
        DVLB_Free(vshader_dvlb);
    }

    void Citro3DShader::Bind()
    {
        C3D_BindProgram(&program);
    }

    void Citro3DShader::Unbind() const
    {
        C3D_BindProgram(0);
    }

    void Citro3DShader::UploadUniformInt(std::string name, int value)
    {
        s8 location = shaderInstanceGetUniformLocation(program.vertexShader, name.c_str());
        C3D_IVUnifSet(GPU_VERTEX_SHADER, location, value, 0, 0, 0);
    }

    void Citro3DShader::UploadUniformFloat(std::string name, const float value)
    {
        s8 location = shaderInstanceGetUniformLocation(program.vertexShader, name.c_str());
        C3D_FVUnifSet(GPU_VERTEX_SHADER, location, value, 0.0f, 0.0f, 0.0f);
    }

    void Citro3DShader::UploadUniformFloat2(std::string name, const glm::vec2& value)
    {
        s8 location = shaderInstanceGetUniformLocation(program.vertexShader, name.c_str());
        C3D_FVUnifSet(GPU_VERTEX_SHADER, location, value.x, value.y, 0.0f, 0.0f);
    }

    void Citro3DShader::UploadUniformFloat3(std::string name, const glm::vec3& value)
    {
        s8 location = shaderInstanceGetUniformLocation(program.vertexShader, name.c_str());
        C3D_FVUnifSet(GPU_VERTEX_SHADER, location, value.x, value.y, value.z, 0.0f);
    }

    void Citro3DShader::UploadUniformFloat4(std::string name, const glm::vec4& value)
    {
        s8 location = shaderInstanceGetUniformLocation(program.vertexShader, name.c_str());
        C3D_FVUnifSet(GPU_VERTEX_SHADER, location, value.x, value.y, value.z, value.w);
    }


    void Citro3DShader::UploadUniformMat3(std::string name, const glm::mat3* matrix)
    {
        C3D_Mtx out_matrix;
        glm_mat3_to_C3D_mtx(&out_matrix, matrix);
        s8 location = shaderInstanceGetUniformLocation(program.vertexShader, name.c_str());
        C3D_FVUnifMtx3x4(GPU_VERTEX_SHADER, location, &out_matrix);
    }

    void Citro3DShader::UploadUniformMat4(std::string name, const C3D_Mtx* matrix)
    {
        s8 location = shaderInstanceGetUniformLocation(program.vertexShader, name.c_str());
        C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, location, matrix);
    }

}