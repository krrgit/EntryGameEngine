#include "etpch.h"
#include "LightEnvironmentPanel.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_internal.h>

namespace Entry
{
	void LightEnvironmentPanel::OnImGuiRender()
	{
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::Begin("Light Environment");
        ImGui::PushFont(boldFont);
        ImGui::Text("Scene");
        ImGui::PopFont();
        ImGui::Separator();

        static glm::vec3 ambientColor = m_Context->GetLightEnvironment()->GetSceneAmbientColor();
        if (ImGui::ColorEdit3("Ambient Color", glm::value_ptr(ambientColor)))
        {
            m_Context->GetLightEnvironment()->SetSceneAmbientColor(ambientColor);
        }

        ImGui::Separator();
        ImGui::PushFont(boldFont);
        ImGui::Text("LUTs");
        ImGui::PopFont();
        ImGui::Separator();


        for (int i = 0; i < 7; i++)
        {
            RenderLut((ET_LIGHTLUTID)i);
        }

        ImGui::Separator();

        ImGui::Text("Light Count: %d", m_Context->GetLightCount());
        ImGui::Text("Light Limit: 8");
        ImGui::End();
	}

    void LightEnvironmentPanel::RenderLut(ET_LIGHTLUTID lutID)
    {
        
        static int ids[] = { 0, 1, -1, 2, 3, 4, 5, -1 };
        int id = ids[(int)lutID];
        if (id < 0) return;

        static std::string lutNames[] = {"Specular0", "Specular1", "Fresnel", "Reflection Red", "Reflection Green", "Reflection Blue" };
        std::string header = "LUT " + id;
        header += ": ";
        header += lutNames[id];

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 2.0f));

        ImGuiTreeNodeFlags flags =  ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
        bool opened = ImGui::TreeNodeEx(lutNames[id].c_str(), flags);
        bool entityDeleted = false;
        ImGui::PopStyleVar(2);


        if (opened)
        {
            //ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
            //bool opened = ImGui::TreeNodeEx((void*)(uint64_t)((uint32_t)entity + 1000), flags, tag.c_str());
            //if (opened)
            //    ImGui::TreePop();

             auto& lutconfig = m_Context->GetLightEnvironment()->GetLutConfig(lutID);

            int funcSelection = (int)lutconfig.funcType;
            const char* functionStrings[] = { "None", "Pow", "Spotlight", "Quadratic", "Toon Diffuse", "Toon Specular", "Custom"};
            const char* currentFunctionString = functionStrings[funcSelection];
            if (ImGui::BeginCombo("LUT Function", currentFunctionString))
            {
                for (int i = 0; i < 6; ++i)
                {
                    bool isSelected = i == funcSelection;
                    if (ImGui::Selectable(functionStrings[i], isSelected))
                    {
                        funcSelection = i;
                        lutconfig.funcType = (LutFuncType)i;
                        m_Context->GetLightEnvironment()->ConfigureLut(lutconfig);
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            static int inputSelection = (int)lutconfig.input;
            const char* inputStrings[] = { 
                "Normal * Half-Vector", 
                "View * Half-Vector", 
                "Normal * View", 
                "Light Vector * Normal", 
                "-Light Vector * Spotlight Vector", 
                "cosine of phi"};
            const char* currentInputString = inputStrings[inputSelection];
            if (ImGui::BeginCombo("LUT Input", currentInputString))
            {
                for (int i = 0; i < 6; ++i)
                {
                    bool isSelected = i == inputSelection;
                    if (ImGui::Selectable(inputStrings[i], isSelected))
                    {
                        inputSelection = i;
                        lutconfig.input = (ET_LIGHTLUTINPUT)i;
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }


            switch (lutconfig.funcType)
            {
            case LutFuncType::Pow:
            {
                ImGui::Separator();
                if (ImGui::InputFloat("Shininess", &lutconfig.funcArgs.powExponent))
                    m_Context->GetLightEnvironment()->ConfigureLut(lutconfig);
            }
             break;
            case LutFuncType::Spotlight:
            {
                ImGui::Separator();
                if (ImGui::DragFloat("Spot Angle", &lutconfig.funcArgs.spotlightCutoff, 1, 1, 179, "%.0f"))
                    m_Context->GetLightEnvironment()->ConfigureLut(lutconfig);
            }
                break;
            case LutFuncType::Quadratic:
            {
                ImGui::Separator();
                ImGui::Text("Attenuation = 1.0 / ( 1.0 + x*d + y*d^2 )");
                if (ImGui::InputFloat("Linear (x)", &lutconfig.funcArgs.quadraticLin))
                    m_Context->GetLightEnvironment()->ConfigureLut(lutconfig);
                if (ImGui::InputFloat("Quad (y)", &lutconfig.funcArgs.quadraticQuad))
                    m_Context->GetLightEnvironment()->ConfigureLut(lutconfig);
            }
                break;
            case LutFuncType::ToonDiffuse:
                {}
                break;
            case LutFuncType::ToonSpecular:
            {
                ImGui::Separator();
                if (ImGui::InputFloat("Shininess", &lutconfig.funcArgs.toonShininess))
                    m_Context->GetLightEnvironment()->ConfigureLut(lutconfig);
            }
                break;
            default:
            break;
            }

            

            ImGui::TreePop();
        }
	}

}

