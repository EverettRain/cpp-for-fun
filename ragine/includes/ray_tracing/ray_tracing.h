#pragma once

#include "ragine.h"

vec3 ray_color(const ray& r, const Hittable& world, int depth);