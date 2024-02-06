#pragma once

#include "Renderer/Transform.h"
#include "Renderer/Frustum.h"
#include "Renderer/BoundingVolume.h"

class Sphere : public BoundingVolume
{
public:
    glm::vec3 center{ 0.f, 0.f, 0.f };
    float radius{ 0.f };

    Sphere(const glm::vec3& inCenter, float inRadius)
        : BoundingVolume{}, center{ inCenter }, radius{ inRadius }
    {}

    bool isOnOrForwardPlane(const Plane& plane) const final
    {
        return plane.getSignedDistanceToPlane(center) > -radius;
    }

    bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const final
    {
        //Get global scale thanks to our transform
        const glm::vec3 globalScale = transform.getGlobalScale();

        //Get our global center with process it with the global model matrix of our transform
        const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };

        //To wrap correctly our shape, we need the maximum scale scalar.
        const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

        //Max scale is assuming for the diameter. So, we need the half to apply it to our radius
        Sphere globalSphere(globalCenter, radius * (maxScale * 0.5f));

        //Check Firstly the result that have the most chance to failure to avoid to call all functions.
        return (globalSphere.isOnOrForwardPlane(camFrustum.leftFace) &&
            globalSphere.isOnOrForwardPlane(camFrustum.rightFace) &&
            globalSphere.isOnOrForwardPlane(camFrustum.farFace) &&
            globalSphere.isOnOrForwardPlane(camFrustum.nearFace) &&
            globalSphere.isOnOrForwardPlane(camFrustum.topFace) &&
            globalSphere.isOnOrForwardPlane(camFrustum.bottomFace));
    };
};