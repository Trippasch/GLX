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

    void SetPosition(const glm::vec3& position) { m_Position = position; }
    void SetOrientation(const glm::vec3& orientation)
    {
        m_Orientation = orientation;
        m_Pitch = glm::degrees(asin(m_Orientation.y));
        m_Yaw = glm::degrees(atan2(m_Orientation.z, m_Orientation.x));
    }
    void SetNearPlane(float nearPlane) { m_NearPlane = nearPlane; }
    void SetFarPlane(float farPlane) { m_FarPlane = farPlane; }
    void SetFov(float fov) { m_Fov = fov; }
    void SetFirstClick(bool firstClick) { m_FirstClick = firstClick; }

    const glm::vec3& GetPosition() const { return m_Position; }
    const glm::vec3& GetOrientation() const { return m_Orientation; }
    const glm::vec3& GetUp() const { return m_Up; }
    const float& GetNearPlane() const { return m_NearPlane; }
    const float& GetFarPlane() const { return m_FarPlane; }
    const float& GetFov() const { return m_Fov; }
    const bool& GetFirstClick() const { return m_FirstClick; }

private:
    glm::vec3 m_Position;
    glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

    float m_FarPlane = 500.0f;
    float m_NearPlane = 0.1f;
    float m_Fov = 45.0f;

    glm::vec3 m_InitialOrientation = glm::vec3(-1.0f, 0.0f, 0.0f);

    // Prevents the camera from jumping around when first clicking left click
    bool m_FirstClick = true;

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