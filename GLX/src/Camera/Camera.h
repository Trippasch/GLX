#pragma once

#include "Camera/Frustum.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const float SPEED = 3.0f;
const float SENSITIVITY = 0.1f;

class Camera
{
public:
    Camera() : m_Width(0), m_Height(0) {}
    Camera(int width, int height, glm::vec3 position);

    glm::mat4 Matrix(float fovDeg, float ratio, float nearPlane, float farPlane);

    inline glm::mat4 &GetViewMatrix() { return m_View; }
    inline glm::mat4 &GetProjectionMatrix() { return m_Projection; }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

    // Handles camera inputs
    void Inputs(GLFWwindow* window, float deltaTime);

    // Create a frustum from the camera
    Frustum CreateFrustumFromCamera(float aspect_ratio, float fovY, float zNear, float zFar);

public:
    glm::vec3 m_Position;
    glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

    float m_FarPlane = 500.0f;
    float m_NearPlane = 0.1f;
    float m_Fov = 45.0f;

    // Prevents the camera from jumping around when first clicking left click
    bool m_FirstClick = true;

private:
    // Stores the width and height of the window
    int m_Width;
    int m_Height;

    // Stores the projection and view matrices
    glm::mat4 m_View;
    glm::mat4 m_Projection;

    // Adjust the speed of the camera and it's sensitivity when looking around
    float m_Speed;
    float m_Sensitivity;

    float m_Yaw = 270.0f;
    float m_Pitch = 0.0f;

    float m_LastX;
    float m_LastY;
};