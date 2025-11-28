#pragma once

#include "ragine.h"
#include <random>

/// @brief 生成 [0, 1) 的随机小数
inline double random_double() {
    static thread_local std::mt19937 generator(std::random_device{}()); 
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}

/// @brief 生成 [min, max) 的随机小数
inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

/// @brief 生成 [0, 1) 的随机三维变量
inline vec3 vec3_random() {
    return {random_double(), random_double(), random_double()};
}

/// @brief 生成 [min, max) 的随机三维变量
inline vec3 vec3_random(double min, double max) {
    return {random_double(min, max), random_double(min, max), random_double(min, max)};
}

/// @brief 在单位球体内生成随机向量 (用于漫反射) 
inline vec3 random_unit_sphere() {
    while (true) {
        vec3 p = vec3_random(-1, 1);
        if (p.dot(p) >= 1) continue;
        return p;
    }
}

/// @brief 生成单位随机向量 (Lambertian 漫反射的标准算法) 
inline vec3 random_unit_vector() {
    return random_unit_sphere().normalize();
}