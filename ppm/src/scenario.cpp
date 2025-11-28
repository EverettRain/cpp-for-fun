#include "ragine.h"

int main() {
    const int width = 800;
    const int height = 600;
    const char* file_path = "ppm/bin/shader_scenario.ppm";

    std::ofstream ofs(file_path, std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";

    // Scenario Definition
    HittableList_legend world;
    vec3 sphere_pos1 = {0, 0, -1};
    vec3 sphere_pos2 = {1, 0, 1};
    Sphere_legend sphere1(sphere_pos1, 0.5, {220.0, 220.0, 220.0});
    Sphere_legend sphere2(sphere_pos2, 0.5, {220.0, 220.0, 220.0});
    Plane_legend ground({0, -2, 0}, {0, 1, 0}, {100.0, 200.0, 100.0});
    world.add(std::make_shared<Sphere_legend>(sphere1));
    world.add(std::make_shared<Sphere_legend>(sphere2));
    world.add(std::make_shared<Plane_legend>(ground));

    // Camera Definition
    vec3 camera_up = { 0.0, 1.0, 0.0 };
    Camera camera({0.0, 4.0, 4.0}, sphere_pos2, camera_up, 60.0, double(width)/double(height) );

    // Light Definition
    vec3 light_pos = {200.0, 200.0, 100.0};
    double shininess = 32;

    for (int y = height - 1; y > -1; y--) {
        for (int x = 0; x < width; x++) {
            double u = (double)x / (width - 1);
            double v = (double)y / (height - 1);

            ray r = camera.get_ray(u, v);

            hit_legend rec;

            if (world.is_hit(r, rec, MINIMUM, INFINITY)) {
                vec3 color;

                vec3 light_dir = (light_pos - rec.position).normalize();

                ray shadow_ray;
                shadow_ray.origin = rec.position;
                shadow_ray.dir = light_dir;

                hit_legend shadow_rec;
                double light_distance = (light_pos - rec.position).length();
                bool in_shadow = world.is_hit(shadow_ray, shadow_rec, MINIMUM, light_distance);

                if (in_shadow) {
                    color = rec.color * 0.02;
                } else {
                    color = blinn_phong_shader(rec, light_pos, camera.position(), rec.color, shininess);
                }

                // Simple Gamma Correction
                color = gamma_correct(color);

                ofs << (unsigned char)std::min(255.0, color.x) << (unsigned char)std::min(255.0, color.y)
                    << (unsigned char)std::min(255.0, color.z);
            } else {
                vec3 unit_direction = r.dir.normalize();
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
