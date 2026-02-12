#include "Cube.h"
#include <array>

void Cube::baseVertices(Vec3 out[8]) {
    static const std::array<Vec3,8> verts = {
        Vec3{-0.5,-0.5,-0.5}, Vec3{0.5,-0.5,-0.5},
        Vec3{0.5, 0.5,-0.5}, Vec3{-0.5, 0.5,-0.5},
        Vec3{-0.5,-0.5, 0.5}, Vec3{0.5,-0.5, 0.5},
        Vec3{0.5, 0.5, 0.5}, Vec3{-0.5, 0.5, 0.5}
    };
    for (int i = 0; i < 8; ++i) out[i] = verts[i];
}
//MAtriz M
Mat4 Cube::modelMatrix() const {
    Mat4 S = Mat4::scale(scale);
    Mat4 Rx = Mat4::rotationX(rotation.x);
    Mat4 Ry = Mat4::rotationY(rotation.y);
    Mat4 Rz = Mat4::rotationZ(rotation.z);
    Mat4 T  = Mat4::translation(position);
    return S * Rx * Ry * Rz * T;
}

