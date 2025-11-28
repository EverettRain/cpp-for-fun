#pragma once

#include "ragine.h"

vec3 debug_normal_shader(const hit_legend& record);
vec3 lambert_shader(const hit_legend& record, const vec3& light_pos, const vec3& obj_color);
vec3 half_lambert_shader(const hit_legend& record, const vec3& light_pos, const vec3& obj_color);
vec3 phong_shader(const hit_legend& record, const vec3& light_pos, const vec3& camera_pos, const vec3& obj_color,
                  const double shininess);
vec3 blinn_phong_shader(const hit_legend& record, const vec3& light_pos, const vec3& camera_pos, const vec3& obj_color,
                        const double shininess);
