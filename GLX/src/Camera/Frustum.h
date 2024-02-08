#pragma once

#include "Camera/FrustumPlane.h"

class Frustum
{
public:
    FrustumPlane topFace;
    FrustumPlane bottomFace;

    FrustumPlane rightFace;
    FrustumPlane leftFace;

    FrustumPlane farFace;
    FrustumPlane nearFace;
};