#include "ragine.h"

vec3 gamma_correct(const vec3& origin) {
    double r = sqrt(origin.x / 255.0) * 255.0;
    double g = sqrt(origin.y / 255.0) * 255.0;
    double b = sqrt(origin.z / 255.0) * 255.0;
    return {r, g, b};
}

vec3 sampled_gamma(const vec3& origin, int sample_times) {
    double scale = 1.0 / sample_times;
    double r = sqrt(origin.x * scale);
    double g = sqrt(origin.y * scale);
    double b = sqrt(origin.z * scale);
    return {r, g, b};
}

/// @brief 计算纯反射光线
/// @param v 入射光线方向
/// @param n 反射点法线方向
/// @return 反射光线方向
vec3 reflect(const vec3& v, const vec3& n) {
    return v - n * (v.dot(n)) * 2;
}

// 计算折射向量 (Snell's Law)
// uv: 
// n: 
// etai_over_etat: 

/// @brief 
/// @param uv 入射光线单位向量
/// @param n 法线单位向量
/// @param etai_over_etat 折射率比值 (n1 / n2)
vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = std::min((uv * (-1.0)).dot(n), 1.0);
    vec3 r_out_perp = (uv + n * cos_theta) * etai_over_etat;
    vec3 r_out_parallel = n * -std::sqrt(std::abs(1.0 - r_out_perp.length_squared()));
    return r_out_perp + r_out_parallel;
}

/// @brief Schlick 近似计算菲涅尔反射率
/// @param cosine 入射角余弦值
/// @param ref_idx 折射率
double reflectance(double cosine, double ref_idx) {
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}