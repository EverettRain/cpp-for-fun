#pragma once

#include "ragine.h"

class aabb {
public:
    vec3 minimum;
    vec3 maximum;
    
    aabb() {}
    aabb(const vec3& min, const vec3& max): minimum(min), maximum(max) {}

    bool is_hit(const ray& r, double t_entry, double t_leave) const {
        for (int axis = 0; axis < 3; axis++) {
            double invD = 1.0 / r.dir[axis];
            double t0 = (minimum[axis] - r.origin[axis]) * invD;
            double t1 = (maximum[axis] - r.origin[axis]) * invD;

            if (invD < 0.0) {
                std::swap(t0, t1);
            }

            t_entry = std::max(t_entry, t0);
            t_leave = std::min(t_leave, t1);

            if (t_entry >= t_leave) return false;
        }
        return true;
    }
};

inline aabb surrounding_box(const aabb& box0, const aabb& box1) {
        vec3 small{
            fmin(box0.minimum.x, box1.minimum.x),
            fmin(box0.minimum.y, box1.minimum.y),
            fmin(box0.minimum.z, box1.minimum.z)
        };

        vec3 big{
            fmax(box0.maximum.x, box1.maximum.x),
            fmax(box0.maximum.y, box1.maximum.y),
            fmax(box0.maximum.z, box1.maximum.z)
        };

        return aabb(small, big);
    }