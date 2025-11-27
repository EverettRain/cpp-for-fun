#pragma once

#include "ragine.h"

class Camera {
    vec3 origin;
    vec3 lower_left;
    vec3 horizontal;
    vec3 vertical;

    bool allow_shadow;
    double focus_dist;

public:
    /// @brief Camera 类构造函数
    /// @param lookfrom 相机位置
    /// @param lookat 相机目标位置
    /// @param vup 相机的正上方向
    /// @param fov 垂直视场角 (度)
    /// @param aspect_ratio 宽高比 (width / height)
    /// @param shadow 是否渲染阴影
    /// @param fd 相机焦距
    Camera (const vec3& lookfrom, const vec3& lookat, const vec3& vup, double fov, double aspect_ratio,
        bool shadow = true, double fd = 1.0) : allow_shadow(shadow), focus_dist(fd) {
        double theta = fov * M_PI / 180.0;
        double half_height = tan(theta / 2.0);
        double half_width = half_height * aspect_ratio;

        origin = lookfrom;
        vec3 w = (lookfrom - lookat).normalize();
        vec3 u = vup.cross(w).normalize();
        vec3 v = w.cross(u);

        horizontal = u * 2 * half_width * focus_dist;
        vertical = v * 2 * half_height * focus_dist;

        lower_left = origin - (horizontal * 0.5) - (vertical * 0.5) - (w * focus_dist);
    }

    ray get_ray(double s, double t) {
        return { origin,
                 lower_left + horizontal * s + vertical * t - origin
        };
    }

    vec3 position() const { return origin; }
};