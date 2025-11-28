#include "ragine.h"
#include <omp.h>

int main() {
    const int width = 800;
    const int height = 600;
    const char* file_path = "ppm/bin/ray_tracing.ppm";

    // Material Definition
    // 灰色漫反射
    auto material_ground = std::make_shared<Lambertian>(Colors::Gray50);
    // 红色金属 fuzz=0.2
    auto material_red = std::make_shared<Lambertian>(vec3{0.7, 0.3, 0.3});
    // 金属 fuzz=0.0
    auto material_fullmetal = std::make_shared<Metal>(vec3{0.8, 0.8, 0.8}, 0.0);
    // 金属 fuzz=0.5
    auto material_halfmetal = std::make_shared<Metal>(vec3{0.8, 0.6, 0.2}, 0.5);
    // 玻璃材质
    auto material_glass = std::make_shared<Dielectric>(vec3{0.9, 0.9, 0.9}, 1.5);

    // Scenario Definition
    vec3 world_up = { 0.0, 1.0, 0.0 };
    HittableList world(std::vector<std::shared_ptr<Hittable>> {
        std::make_shared<Plane>(vec3{0, -0.5, 0}, world_up, material_ground),

        std::make_shared<Plane>(vec3{0, -0.5, 0}, world_up, material_ground),
        std::make_shared<Sphere>(vec3{-1.0, 0.0, -1.0}, 0.5, material_red),
        std::make_shared<Sphere>(vec3{ 0.0, 0.0, -1.0}, 0.5, material_glass),
        std::make_shared<Sphere>(vec3{ 0.0, 0.0, -1.0}, -0.45, material_glass),
        std::make_shared<Sphere>(vec3{ 1.0, 0.0, -1.0}, 0.5, material_fullmetal)
    });

    // Camera Definition
    Camera camera({0.0, 0.3, 2.0}, {0.0, 0.0, -1.0}, world_up, 60.0, double(width)/double(height));

    // Ray Tracing Definition
    const int max_depth = 50;
    const int samples_per_pixel = 200;

    // 多线程处理
    std::vector<vec3> image_buffer(width * height);
    std::cout << "Start rendering with " << omp_get_max_threads() << " threads..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for collapse(2)
    for (int y = height - 1; y > -1; y--) {
        for (int x = 0; x < width; x++) {
            vec3 pixel_color = {0, 0, 0};

            for (int s = 0; s < samples_per_pixel; ++s) {
                // u, v 加上随机偏移量
                double u = (double(x) + random_double()) / (width - 1);
                double v = (double(height - 1 - y) + random_double()) / (height - 1);

                ray r = camera.get_ray(u, v);
                
                // 累加颜色
                pixel_color = pixel_color + ray_color(r, world, max_depth);
            }

            image_buffer[y * width + x] = sampled_gamma(pixel_color, samples_per_pixel);
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Render time: " << elapsed.count() << "s" << std::endl;

    std::ofstream ofs(file_path, std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";

    for (const auto& col : image_buffer) {
         ofs << (unsigned char)(255.99 * std::min(0.999, std::max(0.0, col.x)))
             << (unsigned char)(255.99 * std::min(0.999, std::max(0.0, col.y)))
             << (unsigned char)(255.99 * std::min(0.999, std::max(0.0, col.z)));
    }

    ofs.close();
    std::cout << "Done! Generated " << file_path << std::endl;

    return 0;
}