#include "Renderer/Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

Camera::Camera(int width, int height, glm::vec3 position)
    : m_Width(width), m_Height(height), m_Position(position), m_Speed(SPEED), m_Sensitivity(SENSITIVITY)
{
    m_LastX = static_cast<float>(width) / 2.0f;
    m_LastY = static_cast<float>(height) / 2.0f;
}

glm::mat4 Camera::Matrix(float fovDeg, float ratio, float nearPlane, float farPlane)
{
    // Initializes matrices since otherwise they will be the null matrix
    m_View = glm::mat4(1.0f);
    m_Projection = glm::mat4(1.0f);

    // Makes camera look in the right direction from the right position
    m_View = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
    // Adds perspective to the scene
    m_Projection = glm::perspective(glm::radians(fovDeg), ratio, nearPlane, farPlane);

    return (m_Projection * m_View);
}

void Camera::ProcessMouseScroll(float yoffset)
{
    m_Fov -= static_cast<float>(yoffset);
    if (m_Fov < 1.0f)
        m_Fov = 1.0f;
    if (m_Fov > 90.0f)
        m_Fov = 90.0f;
}

void Camera::Inputs(GLFWwindow* window, float deltaTime)
{
    float velocity = m_Speed * deltaTime;

    // Handles key inputs
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        m_Position += velocity * m_Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        m_Position += velocity * -glm::normalize(glm::cross(m_Orientation, m_Up));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        m_Position += velocity * -m_Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        m_Position += velocity * glm::normalize(glm::cross(m_Orientation, m_Up));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        m_Position += velocity * m_Up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        m_Position += velocity * -m_Up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        m_Speed = SPEED * 4.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    {
        m_Speed = SPEED;
    }

    // Handles mouse inputs
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        // Disables mouse cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Stores the coordinates of the cursor
        double mouseX;
        double mouseY;
        // Fetches the coordinates of the cursor
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float xpos = static_cast<float>(mouseX);
        float ypos = static_cast<float>(mouseY);

        // Prevents camera from jumping on the first click
        if (m_FirstClick) {
            m_LastX = xpos;
            m_LastY = ypos;
            m_FirstClick = false;
        }

        float xoffset = xpos - m_LastX;
        float yoffset = m_LastY - ypos;
        m_LastX = xpos;
        m_LastY = ypos;

        xoffset *= m_Sensitivity;
        yoffset *= m_Sensitivity;

        m_Yaw += xoffset;
        m_Pitch += yoffset;

        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
        
        glm::vec3 direction;
        direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        direction.y = sin(glm::radians(m_Pitch));
        direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Orientation = glm::normalize(direction);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
    {
        // Unhides cursor since camera is not looking around anymore
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Makes sure the next time the camera looks around it doesn't jump
        m_FirstClick = true;
    }
}

Frustum Camera::CreateFrustumFromCamera(float aspect_ratio, float fovY, float zNear, float zFar)
{
    Frustum frustum;
    const float halfVSide = zFar * tanf(fovY * .5f);
    const float halfHSide = halfVSide * aspect_ratio;
    const glm::vec3 frontMultFar = zFar * m_Orientation;

    glm::vec3 camRight = glm::normalize(glm::cross(m_Orientation, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 camUp = glm::normalize(glm::cross(camRight, m_Orientation));

    frustum.nearFace = { m_Position + zNear * m_Orientation, m_Orientation };
    frustum.farFace = { m_Position + frontMultFar, -m_Orientation };
    frustum.rightFace = { m_Position, glm::cross(frontMultFar - camRight * halfHSide, camUp) };
    frustum.leftFace = { m_Position, glm::cross(camUp, frontMultFar + camRight * halfHSide) };
    frustum.topFace = { m_Position, glm::cross(camRight, frontMultFar - camUp * halfVSide) };
    frustum.bottomFace = { m_Position, glm::cross(frontMultFar + camUp * halfVSide, camRight) };
    return frustum;
}