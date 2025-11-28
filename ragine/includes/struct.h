#pragma once

#include <cmath>
#include <memory>

class Material;

struct vec3 {
    double x, y, z;

    vec3 operator+(const vec3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    vec3 operator-(const vec3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    vec3 operator*(const vec3& v) const { return {x * v.x, y * v.y, z * v.z}; }
    vec3 operator*(const double t) const { return {x * t, y * t, z * t}; }

    vec3 normalize() const {
        double len = sqrt(x * x + y * y + z * z);
        return {x / len, y / len, z / len};
    }
    vec3 cross(const vec3& v) const { 
        return {
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        };
    }
    vec3 up() const { return { 0.0, 1.0, 0.0 }; }

    double dot(const vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    double length_squared() const { return x * x + y * y + z * z; }
    double length() const { return sqrt(x * x + y * y + z * z); }
};

struct hit_legend {
    vec3 position;
    vec3 normal;
    vec3 color;
    double time;
};

struct hit {
    vec3 position;
    vec3 normal;
    Material* material;
    double time;
};

struct ray {
    vec3 origin;
    vec3 dir;
    vec3 at(const double time) const { return origin + dir * time; }
};
