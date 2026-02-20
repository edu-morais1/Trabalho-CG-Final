#include "Camera.h"
#include "../pipeline/Pipeline.h"

Mat4 Camera::viewMatrix() const {
    return Pipeline::B(u, v, n) * Pipeline::A(vrp);
}

Mat4 Camera::projectionMatrix() const {
    return Pipeline::P(farPlane, nearPlane)
         * Pipeline::D(Su, Sv, DP, farPlane)
         * Pipeline::C(Cu, Cv, DP);
}

