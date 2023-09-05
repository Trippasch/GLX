#pragma once

#include <memory>

#include "Core/Window.h"
#include "ImGui/ImGuiLayer.h"
#include "Sandbox/SandboxLayer.h"
#include "Core/LayerStack.h"
#include "Core/Console.h"

class Application
{
public:
    Application();
    virtual ~Application();

    void PushLayer(Layer *layer);
    void PushOverlay(Layer *layer);

    inline ImGuiLayer *GetImGuiLayer() { return m_ImGuiLayer; }
    inline SandboxLayer* GetSandboxLayer() { return m_SandboxLayer; }

    void Run();

    inline Window& GetWindow() { return *m_Window; }
    inline static Application& Get() { return *s_Instance; }

    Console m_Console;

private:
    std::unique_ptr<Window> m_Window;
    ImGuiLayer *m_ImGuiLayer;
    SandboxLayer *m_SandboxLayer;
    LayerStack m_LayerStack;
    bool m_Running = true;
    static Application* s_Instance;
};