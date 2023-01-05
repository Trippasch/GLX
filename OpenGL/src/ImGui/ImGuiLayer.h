#pragma once

#include "Core/Layer.h"

class ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnImGuiRender() override;

    void Begin();
    void End();

    void SetDraculaTheme();
    void SetDarkThemeColors();
};
