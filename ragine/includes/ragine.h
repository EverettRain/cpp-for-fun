#pragma once

/************************************************** 

RAGINE - PPM GENERATE ENGINE

Created by Everett Rain 2025-11-27 (MIT License)

**************************************************/

// RAGINE - Mathematical Macros
#define MAXIMUM 1000.0
#define MINIMUM 0.0001

// C++ Standard Library STL
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>

// RAGINE - Components
#include "components/struct.h"
#include "components/utils.h"
#include "components/component.h"
#include "components/random.h"
#include "components/texture.h"

// RAGINE - Legend APIs
#include "legend/shader.h"
#include "legend/object_legend.h"

// RAGINE - BVH Optimization
#include "bvh/aabb.h"
#include "bvh/bvh.h"

// RAGINE - Ray Tracing
#include "ray_tracing/material.h"
#include "ray_tracing/object.h"
#include "ray_tracing/ray_tracing.h"