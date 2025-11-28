#include "ragine.h"

vec3 debug_normal_shader(const hit_legend& record) {
    // 法线范围是 -1 到 1，颜色范围是 0 到 255 (或者 0 到 1), 映射到 0 ~ 1 之间以便观察
    return (record.normal + vec3{1, 1, 1}) * 0.5 * 255.0;
}

vec3 lambert_shader(const hit_legend& record, const vec3& light_pos, const vec3& obj_color) {
    // Lambert Shader
    vec3 light_dir = (light_pos - record.position).normalize();
    double intensity = std::max(0.0, record.normal.dot(light_dir));
    return obj_color * intensity;
}

vec3 half_lambert_shader(const hit_legend& record, const vec3& light_pos, const vec3& obj_color) {
    // Half Lambert Shader
    vec3 light_dir = (light_pos - record.position).normalize();
    double intensity = record.normal.dot(light_dir) * 0.5 + 0.5;
    return obj_color * intensity;
}

// Phong : Light = Ambient + Diffuse + Specular
vec3 phong_shader(const hit_legend& record, const vec3& light_pos, const vec3& camera_pos, const vec3& obj_color,
                  const double shininess) {
    vec3 light_dir = (light_pos - record.position).normalize();
    vec3 camera_dir = (camera_pos - record.position).normalize();
    vec3 reflect_dir = record.normal * 2.0 * record.normal.dot(light_dir) - light_dir;

    // Ambient
    vec3 ambient = obj_color * 0.05;

    // Diffuse
    double diff_val = std::max(0.0, record.normal.dot(light_dir));

    // Specular
    double spec_angle = std::max(0.0, camera_dir.dot(reflect_dir));
    double spec_val = std::pow(spec_angle, shininess);

    vec3 diffuse = obj_color * diff_val;
    vec3 specular = vec3{255.0, 255.0, 255.0} * spec_val * (diff_val > 0.0 ? 1.0 : 0.0);

    return ambient + diffuse + specular;
}

// Blinn Phong
vec3 blinn_phong_shader(const hit_legend& record, const vec3& light_pos, const vec3& camera_pos, const vec3& obj_color,
                        const double shininess) {
    vec3 light_dir = (light_pos - record.position).normalize();
    vec3 camera_dir = (camera_pos - record.position).normalize();

    // Ambient
    vec3 ambient = obj_color * 0.05;

    // Diffuse
    double diff_val = std::max(0.0, record.normal.dot(light_dir));

    // Specular
    vec3 half = (light_dir + camera_dir).normalize();
    double spec_angle = std::max(0.0, half.dot(record.normal));
    double spec_val = std::pow(spec_angle, shininess * 2.0);

    vec3 diffuse = obj_color * diff_val;
    vec3 specular = vec3{255.0, 255.0, 255.0} * spec_val * (diff_val > 0.0 ? 1.0 : 0.0);

    return ambient + diffuse + specular;
}
