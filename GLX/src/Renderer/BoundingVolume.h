#pragma once

#include "Renderer/Transform.h"
#include "Renderer/Frustum.h"

class BoundingVolume
{
public:
    virtual bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const = 0;

    virtual bool isOnOrForwardPlane(const Plane& plane) const = 0;

    bool isOnFrustum(const Frustum& camFrustum) const
    {
        return (isOnOrForwardPlane(camFrustum.leftFace) &&
            isOnOrForwardPlane(camFrustum.rightFace) &&
            isOnOrForwardPlane(camFrustum.topFace) &&
            isOnOrForwardPlane(camFrustum.bottomFace) &&
            isOnOrForwardPlane(camFrustum.nearFace) &&
            isOnOrForwardPlane(camFrustum.farFace));
    };
};