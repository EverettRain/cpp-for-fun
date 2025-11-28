#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

struct vec3 {
    double x, y, z;

    vec3 operator+(const vec3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    vec3 operator-(const vec3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    vec3 operator*(const double t) const { return {x * t, y * t, z * t}; }

    vec3 normalize() const {
        double len = sqrt(x * x + y * y + z * z);
        return {x / len, y / len, z / len};
    }

    double dot(const vec3& v) const { return x * v.x + y * v.y + z * v.z; }
};

struct hit_legend {
    vec3 position;
    vec3 normal;
};

bool hit_sphere(hit_legend& hit_result, const vec3& center, const vec3& origin, const vec3& dir, const double radius) {
    vec3 co = origin - center;
    double a = dir.dot(dir);
    double b = 2.0 * co.dot(dir);
    double c = co.dot(co) - radius * radius;
    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return false;

    double sqrtd = std::sqrt(discriminant);

    double root = (-b - sqrtd) / (2.0 * a);

    if (root < 0.001) {
        root = (-b + sqrtd) / (2.0 * a);
        if (root < 0.001) return false;
    }

    hit_result.position = origin + dir * root;
    // 技巧：除以半径通常比调用 normalize() (开根号) 快
    hit_result.normal = (hit_result.position - center) * (1.0 / radius);

    return true;
}

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

int main() {
    const int width = 800;
    const int height = 600;
    const char* file_path = "ppm/bin/sphere_shader_blinn_phong1.ppm";

    std::ofstream ofs(file_path, std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";

    // Scenario Definition
    vec3 cam_pos = {0, 0, 0};
    vec3 sphere_pos = {0, 0, -1};
    vec3 sphere_color = {220.0, 220.0, 220.0};
    double sphere_radius = 0.5;

    // Camera Definition
    vec3 lower_left = {-2.0, -1.5, -1.0};
    vec3 horizontal = {4.0, 0.0, 0.0};
    vec3 vertical = {0.0, 3.0, 0.0};

    // Light Definition
    vec3 light_pos = {2.0, 2.0, 1.0};
    double shininess = 32;

    // Screen Scanning Loop
    for (int y = height - 1; y > -1; y--) {
        for (int x = 0; x < width; x++) {
            double u = (double)x / (width - 1);
            double v = (double)y / (height - 1);

            vec3 ray_dir = (lower_left + horizontal * u + vertical * v - cam_pos).normalize();

            hit_legend rec;

            if (hit_sphere(rec, sphere_pos, cam_pos, ray_dir, sphere_radius)) {
                // vec3 color = debug_normal_shader(rec);
                vec3 phong = blinn_phong_shader(rec, light_pos, cam_pos, sphere_color, shininess);
                // Simple Gamma Correction
                double r = sqrt(phong.x / 255.0) * 255.0;
                double g = sqrt(phong.y / 255.0) * 255.0;
                double b = sqrt(phong.z / 255.0) * 255.0;

                ofs << (unsigned char)std::min(255.0, r) << (unsigned char)std::min(255.0, g)
                    << (unsigned char)std::min(255.0, b);
            } else {
                vec3 unit_direction = ray_dir.normalize();
                double t = 0.5 * (unit_direction.y + 1.0);
                // Background Color : White & Blue
                unsigned char r = static_cast<unsigned char>(255 * (1.0 - t) + 0.5 * 255 * t);
                unsigned char g = static_cast<unsigned char>(255 * (1.0 - t) + 0.7 * 255 * t);
                unsigned char b = static_cast<unsigned char>(255 * (1.0 - t) + 1.0 * 255 * t);
                ofs << r << g << b;
            }
        }
    }

    ofs.close();
    std::cout << "Done! Generated " << file_path << std::endl;

    return 0;
}
