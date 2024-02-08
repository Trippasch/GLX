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
        0, 1, 2, 3, 4, 5, 6, 7,  // Front and back face edges
        0, 2, 1, 3, 4, 6, 5, 7,  // Top and bottom face edges
        0, 4, 1, 5, 2, 6, 3, 7   // Vertical edges
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

    std::array<glm::vec3, 8> getVertice() const
    {
        std::array<glm::vec3, 8> vertice;
        vertice[0] = { center.x - extents.x, center.y - extents.y, center.z - extents.z };
        vertice[1] = { center.x + extents.x, center.y - extents.y, center.z - extents.z };
        vertice[2] = { center.x - extents.x, center.y + extents.y, center.z - extents.z };
        vertice[3] = { center.x + extents.x, center.y + extents.y, center.z - extents.z };
        vertice[4] = { center.x - extents.x, center.y - extents.y, center.z + extents.z };
        vertice[5] = { center.x + extents.x, center.y - extents.y, center.z + extents.z };
        vertice[6] = { center.x - extents.x, center.y + extents.y, center.z + extents.z };
        vertice[7] = { center.x + extents.x, center.y + extents.y, center.z + extents.z };
        return vertice;
    }

    void drawAABBSetup() {
        std::array<glm::vec3, 8> vertices = getVertice();
        vbo = VertexBuffer(&vertices, sizeof(vertices), GL_STATIC_DRAW);
        ebo = IndexBuffer(&edges, sizeof(edges), GL_STATIC_DRAW);
    }

    void drawAABB(Shader& shader, glm::mat4 modelMatrix) {
        std::array<glm::vec3, 8> vertices = getVertice();
        shader.Use().SetMatrix4(1, modelMatrix);
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