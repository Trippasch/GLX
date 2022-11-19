#pragma once

#include <memory>

#include "Core/Window.h"
#include "ImGui/ImGuiLayer.h"
#include "Core/LayerStack.h"

class Application
{
public:
    Application();
    virtual ~Application();

    void PushLayer(Layer *layer);
    void PushOverlay(Layer *layer);

    ImGuiLayer *GetImGuiLayer() { return m_ImGuiLayer; }

    void Run();

    inline Window& GetWindow() { return *m_Window; }
    inline static Application& Get() { return *s_Instance; }

private:
    std::unique_ptr<Window> m_Window;
    ImGuiLayer *m_ImGuiLayer;
    LayerStack m_LayerStack;
    bool m_Running = true;
    static Application* s_Instance;
};