#pragma once

#include <string>

#include <GLFW/glfw3.h>

struct WindowProps
{
    std::string Title;
    uint32_t Width;
    uint32_t Height;

    WindowProps(const std::string& title = "GLX",
        uint32_t width = 1280,
        uint32_t height = 720)
        : Title(title), Width(width), Height(height) {}
};

class Window
{
public:
    Window(const WindowProps& props);
    ~Window();

    void OnUpdate();

    inline uint32_t& GetWidth() { return m_Data.Width; }
    inline uint32_t& GetHeight() { return m_Data.Height; }
    inline void SetWidth(uint32_t width) { m_Data.Width = width; }
    inline void SetHeight(uint32_t height) { m_Data.Height = height; }

    inline uint32_t& GetXPos() { return m_Data.XPos; }
    inline uint32_t& GetYPos() { return m_Data.YPos; }
    inline void SetXPos(uint32_t xpos) { m_Data.XPos = xpos; }
    inline void SetYPos(uint32_t ypos) { m_Data.YPos = ypos; }

    // Window attributes
    // inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
    void SetVSync(bool enabled);
    bool IsVSync() const;

    inline virtual GLFWwindow* GetNativeWindow() const { return m_Window; }

    static Window* Create(const WindowProps& props = WindowProps());
private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();
private:
    GLFWwindow* m_Window;

    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        unsigned int XPos, YPos;
        bool VSync;

        // EventCallbackFn EventCallback;
    };
    WindowData m_Data;
};