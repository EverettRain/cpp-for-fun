#include "ragine.h"
#include <omp.h>

HittableList random_world() {
    HittableList world;

    auto checker = std::make_shared<CheckerTexture>(vec3{0.0, 0.0, 0.0}, vec3{0.9, 0.9, 0.9});
    auto ground_material = std::make_shared<Lambertian>(checker);
    world.add(std::make_shared<Plane>(vec3{0, 0, 0}, vec3{0, 1, 0}, ground_material));

    HittableList balls_list;

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            // 随机位置
            double choose_mat = random_double();
            vec3 center{a + 0.9 * random_double(), 0.2, b + 0.9 * random_double()};

            // 避免小球和大球位置重叠
            if ((center - vec3{4, 0.2, 0}).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // 漫反射 (80% 概率)
                    // 颜色向量相乘 = 使得颜色更柔和/偏暗，减少刺眼的亮色
                    auto albedo = vec3_random() * vec3_random();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    balls_list.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // 金属 (15% 概率)
                    auto albedo = vec3_random(0.5, 1); // 金属颜色通常较亮
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    balls_list.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // 玻璃 (5% 概率)
                    sphere_material = std::make_shared<Dielectric>(vec3{1.0, 1.0, 1.0}, 1.5);
                    balls_list.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    // 3. 三个大球
    // 玻璃球
    auto material1 = std::make_shared<Dielectric>(vec3{1.0, 1.0, 1.0}, 1.5);
    balls_list.add(std::make_shared<Sphere>(vec3{0, 1, 0}, 1.0, material1));

    // 漫反射球
    auto earth_texture = std::make_shared<ImageTexture>("ppm/res/earth_texture.jpg", vec2{1.0, 1.0}, vec2{0.25, 0.0});
    auto earth_surface = std::make_shared<Lambertian>(earth_texture);

    auto material2 = std::make_shared<Lambertian>(vec3{0.4, 0.2, 0.1});
    balls_list.add(std::make_shared<Sphere>(vec3{4, 1, 2}, 1.0, earth_surface));

    // 金属球
    auto material3 = std::make_shared<Metal>(vec3{0.7, 0.6, 0.5}, 0.0);
    balls_list.add(std::make_shared<Sphere>(vec3{4, 1, 0}, 1.0, material3));

    std::cout << "Creating BVH from " << balls_list.objects.size() << " spheres..." << std::endl;
    
    if (balls_list.objects.empty()) {
        std::cerr << "ERROR: balls_list is empty!" << std::endl;
    }

    auto bvh_root = std::make_shared<bvh_node>(balls_list, 0.0, 1.0);
    world.add(bvh_root);

    std::cout << "BVH Root Box: " 
              << "Min(" << bvh_root->box.minimum.x << ", " << bvh_root->box.minimum.y << ", " << bvh_root->box.minimum.z << ") "
              << "Max(" << bvh_root->box.maximum.x << ", " << bvh_root->box.maximum.y << ", " << bvh_root->box.maximum.z << ")" 
              << std::endl;

    return world;
}

int main() {
    const int width = 1920;
    const int height = 1080;
    const char* file_path = "ppm/bin/final_scene.ppm";

    std::cout << "Generating scene..." << std::endl;
    HittableList world = random_world();

    vec3 lookfrom = {13, 2, 3};
    vec3 lookat = {0, 0, 0};
    vec3 vup = {0, 1, 0};
    // 20度视场角 = 长焦镜头，适合拍大场景特写
    // 0.1 光圈 (focus_dist 设为 10.0，也就是 lookfrom 到 lookat 的距离)
    double dist_to_focus = 10.0;
    Camera camera(lookfrom, lookat, vup, 20.0, double(width)/height, dist_to_focus);

    const int max_depth = 50;
    const int samples_per_pixel = 200;

    // 多线程处理
    std::vector<vec3> image_buffer(width * height);
    
    std::cout << "Start rendering " << world.get_size() << " objects with " << omp_get_max_threads() << " threads..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for collapse(2) schedule(dynamic) // dynamic 调度有助于负载均衡
    for (int y = height - 1; y > -1; y--) {
        for (int x = 0; x < width; x++) {
            vec3 pixel_color = {0, 0, 0};
            for (int s = 0; s < samples_per_pixel; ++s) {
                double u = (double(x) + random_double()) / (width - 1);
                double v = (double(height - 1 - y) + random_double()) / (height - 1);
                ray r = camera.get_ray(u, v);
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