#pragma once

#include <cmath>

struct vec3 {
    double x, y, z;

    vec3 operator+(const vec3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    vec3 operator-(const vec3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    vec3 operator*(const double t) const { return {x * t, y * t, z * t}; }

    vec3 normalize() const {
        double len = sqrt(x * x + y * y + z * z);
        return {x / len, y / len, z / len};
    }

    double dot(const vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    double length() const { return sqrt(x * x + y * y + z * z); }
};

struct hit {
    vec3 position;
    vec3 normal;
    vec3 color;
    double time;
};

struct ray {
    vec3 origin;
    vec3 dir;
    vec3 at(const double time) const { return origin + dir * time; }
};
