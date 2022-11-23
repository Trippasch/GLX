#include "Core/Window.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Core/Log.h"
#include "Core/Core.h"

#include "Utils/glfw_tools.h"
#include "Utils/gl_tools.h"

static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char *description)
{
    GL_ERROR("GLFW Error {0}: {1}", error, description);
}

Window* Window::Create(const WindowProps& props)
{
    return new Window(props);
}

Window::Window(const WindowProps& props)
{
    Init(props);
}

Window::~Window()
{
    Shutdown();
}

void Window::Init(const WindowProps& props)
{
    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    GL_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    if (!s_GLFWInitialized)
    {
        // TODO: glfwTerminate on system shutdown
        int success = glfwInit();
        GL_ASSERT(success, "Could not initialize GLFW!");
        glfwSetErrorCallback(GLFWErrorCallback);
        s_GLFWInitialized = true;
    }

    // These hints switch the OpenGL profile to core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
    GL_ASSERT(m_Window, "Could not create GLFW window");

    /* Set the window's position */
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    glfwSetWindowPos(m_Window, (mode->width - m_Data.Width) / 2, (mode->height - m_Data.Height) / 2);

    /* Make the window's context curren */
    glfwMakeContextCurrent(m_Window);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    GL_ASSERT(status, "Could not initialize Glad.");

    printGLFWInfo(GetNativeWindow());
    printGLInfo();

    glfwSetWindowUserPointer(m_Window, &m_Data);
    SetVSync(false);

    // Set important OpenGL states
    glEnable(GL_DEPTH_TEST);

    // Set GLFW callbacks
    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
    });

    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        GL_TRACE("Resizing window to {0}x{1}", width, height);
        glViewport(0, 0, width, height);

        Window& w = *(Window*)glfwGetWindowUserPointer(window);
        w.SetWidth(width);
        w.SetHeight(height);

        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.Width = width;
        data.Height = height;
    });

    // glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
    //     {
    //         WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
    //         data.Width = width;
    //         data.Height = height;

    //         WindowResizeEvent event(width, height);
    //         data.EventCallback(event);
    //     });

    // glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
    //     {
    //         WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

    //         WindowCloseEvent event;
    //         data.EventCallback(event);
    //     });

    // glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    //     {
    //         WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

    //         switch (action)
    //         {
    //             case GLFW_PRESS:
    //             {
    //                 KeyPressedEvent event(key, 0);
    //                 data.EventCallback(event);
    //                 break;
    //             }
    //             case GLFW_RELEASE:
    //             {
    //                 KeyReleasedEvent event(key);
    //                 data.EventCallback(event);
    //                 break;
    //             }
    //             case GLFW_REPEAT:
    //             {
    //                 KeyPressedEvent event(key, 1);
    //                 data.EventCallback(event);
    //                 break;
    //             }
    //         }
    //     });

    // glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
    //     {
    //         WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

    //         KeyTypedEvent event(keycode);
    //         data.EventCallback(event);
    //     });

    // glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
    //     {
    //         WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

    //         switch (action)
    //         {
    //             case GLFW_PRESS:
    //             {
    //                 MouseButtonPressedEvent event(button);
    //                 data.EventCallback(event);
    //                 break;
    //             }
    //             case GLFW_RELEASE:
    //             {
    //                 MouseButtonReleasedEvent event(button);
    //                 data.EventCallback(event);
    //                 break;
    //             }
    //         }
    //     });

    // glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
    //     {
    //         WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

    //         MouseScrolledEvent event((float)xOffset, (float)yOffset);
    //         data.EventCallback(event);
    //     });

    // glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
    //     {
    //         WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

    //         MouseMovedEvent event((float)xPos, (float)yPos);
    //         data.EventCallback(event);
    //     });
}

void Window::Shutdown()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Window::OnUpdate()
{
    /* Swap front and back buffers */
    glfwSwapBuffers(m_Window);
    /* Poll for and process events */
    glfwPollEvents();
}

void Window::SetVSync(bool enabled)
{
    if (enabled)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);

    m_Data.VSync = enabled;
}

bool Window::IsVSync() const
{
    return m_Data.VSync;
}