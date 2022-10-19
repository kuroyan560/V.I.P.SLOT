#include "ConstParameters.h"
#include"imgui.h"
#include<magic_enum.h>
#include<functional>
#include<array>

float ConstParameter::Environment::FIELD_WIDTH = 60.0f;
float ConstParameter::Environment::FIELD_HEIGHT_MAX = 25.5f;
float ConstParameter::Environment::FIELD_HEIGHT_MIN = 0.0f;

void ConstParameter::ImguiDebug()
{
    static enum NAME_SPACE
    {
        Environment,
        Player,
        Slot,
        GameObject,
        Stage,
        Num
    }selectNameSpace = Environment;

    static std::array<std::function<void()>, static_cast<int>(NAME_SPACE::Num)> imguiFuncs =
    {
        Environment::ImGuiDebug,
        Player::ImGuiDebug,
        Slot::ImGuiDebug,
        GameObject::ImGuiDebug,
        Stage::ImGuiDebug
    };


    ImGui::Begin("ConstParameters", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("namespace"))
        {
            for (int i = 0; i < NAME_SPACE::Num; ++i)
            {
                NAME_SPACE space = static_cast<NAME_SPACE>(i);
                if (ImGui::MenuItem(std::string(magic_enum::enum_name(space)).c_str()))
                {
                    selectNameSpace = space;
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    auto nowSpaceName = std::string(magic_enum::enum_name(selectNameSpace));
    ImGui::Text("%s", nowSpaceName.c_str());
    imguiFuncs[static_cast<int>(selectNameSpace)]();

    ImGui::End();
}

void ConstParameter::Environment::ImGuiDebug()
{
    ImGui::DragFloat("FIELD_WIDTH", &FIELD_WIDTH, 0.5f);
    ImGui::DragFloat("FIELD_HEIGHT_MAX", &FIELD_HEIGHT_MAX, 0.5f);
    ImGui::DragFloat("FIELD_HEIGHT_MIN", &FIELD_HEIGHT_MIN, 0.5f);
}

void ConstParameter::Player::ImGuiDebug()
{
}

void ConstParameter::Slot::ImGuiDebug()
{
}

void ConstParameter::GameObject::ImGuiDebug()
{
}

void ConstParameter::Stage::ImGuiDebug()
{
}
