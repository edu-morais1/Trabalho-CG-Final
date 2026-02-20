#pragma once

#include "../math/Matrix.h"
#include "../math/Vector.h"

struct Pipeline {
    static Mat4 A(const Vec3 &vrp);

    
    static Mat4 B(const Vec3 &u, const Vec3 &v, const Vec3 &n);


    static Mat4 C(double Cu, double Cv, double d);


    static Mat4 D(double Su, double Sv, double d, double f);


    static Mat4 P(double farPlane, double nearPlane);


    static Mat4 J();

 
    static Mat4 K();

 
    static Mat4 L(double x_max, double x_min,
                  double y_max, double y_min,
                  double z_max, double z_min);


    static Mat4 M();
};

