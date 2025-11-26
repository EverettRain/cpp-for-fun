#include "ragine.h"

int main() {
    const int width = 800;
    const int height = 600;
    const char* file_path = "ppm/bin/shader_scenario.ppm";

    std::ofstream ofs(file_path, std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";

    // Scenario Definition
    HittableList world;
    Sphere sphere({0, 0, -1}, 0.5, {220.0, 220.0, 220.0});
    Plane ground({0, -2, 0}, {0, 1, 0}, {100.0, 200.0, 100.0});
    world.add(std::make_shared<Sphere>(sphere));
    world.add(std::make_shared<Plane>(ground));

    // Camera Definition
    vec3 lower_left = {-2.0, -1.5, -1.0};
    vec3 horizontal = {4.0, 0.0, 0.0};
    vec3 vertical = {0.0, 3.0, 0.0};
    vec3 cam_pos = {0, 0, 0};
    ray r;
    r.origin = cam_pos;

    // Light Definition
    vec3 light_pos = {2.0, 2.0, 1.0};
    double shininess = 32;

    for (int y = height - 1; y > -1; y--) {
        for (int x = 0; x < width; x++) {
            double u = (double)x / (width - 1);
            double v = (double)y / (height - 1);

            r.dir = (lower_left + horizontal * u + vertical * v - cam_pos).normalize();

            hit rec;

            if (world.is_hit(r, rec, MINIMUM, INFINITY)) {
                vec3 phong = blinn_phong_shader(rec, light_pos, cam_pos, rec.color, shininess);
                // Simple Gamma Correction
                double r = sqrt(phong.x / 255.0) * 255.0;
                double g = sqrt(phong.y / 255.0) * 255.0;
                double b = sqrt(phong.z / 255.0) * 255.0;

                ofs << (unsigned char)std::min(255.0, r) << (unsigned char)std::min(255.0, g)
                    << (unsigned char)std::min(255.0, b);
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
