#pragma once

#include "Mesh/Transform.h"
#include "Camera/Frustum.h"
#include "Mesh/BoundingVolume.h"

#include "Resources/VertexBuffer.h"
#include "Resources/IndexBuffer.h"

#include <array>

class AABB : public BoundingVolume
{
private:
    VertexBuffer vbo;
    IndexBuffer ebo;

    std::array<int, 24> edges = {
        0, 1, 1, 3, 3, 2, 2, 0,  // Back face edges
        4, 5, 5, 7, 7, 6, 6, 4,  // Front face edges
        0, 4, 1, 5, 3, 7, 2, 6   // Edges connecting front and back faces
    };

public:
    glm::vec3 center{ 0.f, 0.f, 0.f };
    glm::vec3 extents{ 0.f, 0.f, 0.f };

    AABB(const glm::vec3& min, const glm::vec3& max)
        : BoundingVolume{}, center{ (max + min) * 0.5f }, extents{ max.x - center.x, max.y - center.y, max.z - center.z }
    {}

    AABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
        : BoundingVolume{}, center{ inCenter }, extents{ iI, iJ, iK }
    {}

    ~AABB()
    {
        vbo.Destroy();
        ebo.Destroy();
    }

    std::array<glm::vec3, 8> getVertices() const
    {
        std::array<glm::vec3, 8> vertice;
        vertice[0] = { center.x - extents.x, center.y - extents.y, center.z - extents.z }; // Bottom-left-back
        vertice[1] = { center.x + extents.x, center.y - extents.y, center.z - extents.z }; // Bottom-right-back
        vertice[2] = { center.x - extents.x, center.y + extents.y, center.z - extents.z }; // Top-left-back
        vertice[3] = { center.x + extents.x, center.y + extents.y, center.z - extents.z }; // Top-right-back
        vertice[4] = { center.x - extents.x, center.y - extents.y, center.z + extents.z }; // Bottom-left-front
        vertice[5] = { center.x + extents.x, center.y - extents.y, center.z + extents.z }; // Bottom-right-front
        vertice[6] = { center.x - extents.x, center.y + extents.y, center.z + extents.z }; // Top-left-front
        vertice[7] = { center.x + extents.x, center.y + extents.y, center.z + extents.z }; // Top-right-front
        return vertice;
    }

    void drawAABBSetup() {
        std::array<glm::vec3, 8> vertices = getVertices();
        vbo = VertexBuffer(&vertices, sizeof(vertices), GL_STATIC_DRAW);
        ebo = IndexBuffer(&edges, sizeof(edges), GL_STATIC_DRAW);
    }

    void drawAABB(glm::mat4 modelMatrix) {
        ResourceManager::GetShader("gizmo").Use().SetMatrix4(1, modelMatrix);
        ebo.Bind();
        vbo.LinkAttrib(0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
        vbo.UnlinkAttrib(0);
        ebo.UnBind();
    }

    //see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
    bool isOnOrForwardPlane(const FrustumPlane& plane) const final
    {
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) +
            extents.z * std::abs(plane.normal.z);

        return -r <= plane.getSignedDistanceToFrustumPlane(center);
    }

    bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const final
    {
        //Get global scale thanks to our transform
        const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };

        // Scaled orientation
        const glm::vec3 right = transform.getRight() * extents.x;
        const glm::vec3 up = transform.getUp() * extents.y;
        const glm::vec3 forward = transform.getForward() * extents.z;

        const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

        const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

        const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

        const AABB globalAABB(globalCenter, newIi, newIj, newIk);

        return (globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.farFace));
    };
};