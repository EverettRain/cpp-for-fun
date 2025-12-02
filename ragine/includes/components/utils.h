#pragma once

#include "ragine.h"

vec3 gamma_correct(const vec3& origin);
vec3 sampled_gamma(const vec3& origin, int sample_times);
vec3 reflect(const vec3& v, const vec3& n);
vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat);
double reflectance(double cosine, double ref_idx);

struct Colors {
    inline static const vec3 Black   {0.0, 0.0, 0.0};
    inline static const vec3 White   {1.0, 1.0, 1.0};
    inline static const vec3 Red     {1.0, 0.0, 0.0};
    inline static const vec3 Green   {0.0, 1.0, 0.0};
    inline static const vec3 Blue    {0.0, 0.0, 1.0};
    inline static const vec3 Yellow  {1.0, 1.0, 0.0};
    inline static const vec3 Cyan    {0.0, 1.0, 1.0};
    inline static const vec3 Magenta {1.0, 0.0, 1.0};
    
    // 常用灰度
    inline static const vec3 Gray20  {0.2, 0.2, 0.2};
    inline static const vec3 Gray50  {0.5, 0.5, 0.5};
    inline static const vec3 Gray80  {0.8, 0.8, 0.8};
};