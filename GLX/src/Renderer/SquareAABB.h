#pragma once

#include "Renderer/Transform.h"
#include "Renderer/Frustum.h"
#include "Renderer/BoundingVolume.h"

class SquareAABB : public BoundingVolume
{
public:
    glm::vec3 center{ 0.f, 0.f, 0.f };
    float extent{ 0.f };

    SquareAABB(const glm::vec3& inCenter, float inExtent)
        : BoundingVolume{}, center{ inCenter }, extent{ inExtent }
    {}

    bool isOnOrForwardPlane(const Plane& plane) const final
    {
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        const float r = extent * (std::abs(plane.normal.x) + std::abs(plane.normal.y) + std::abs(plane.normal.z));
        return -r <= plane.getSignedDistanceToPlane(center);
    }

    bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const final
    {
        //Get global scale thanks to our transform
        const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };

        // Scaled orientation
        const glm::vec3 right = transform.getRight() * extent;
        const glm::vec3 up = transform.getUp() * extent;
        const glm::vec3 forward = transform.getForward() * extent;

        const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

        const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

        const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

        const SquareAABB globalAABB(globalCenter, std::max(std::max(newIi, newIj), newIk));

        return (globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.farFace));
    };
};