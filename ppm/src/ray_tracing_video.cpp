#include <iomanip> 
#include <sstream> 
#include "ragine.h"
#include <omp.h>

int main() {
    const int width = 800;
    const int height = 600;

    // 多线程像素处理缓存
    std::vector<vec3> image_buffer(width * height);

    // Material Definition
    // 地面材质：灰色漫反射
    auto material_ground = std::make_shared<Lambertian>(Colors::Gray50);
    // 中间球：红色金属 fuzz=0.2
    auto material_red = std::make_shared<Lambertian>(vec3{0.7, 0.3, 0.3});
    // 左边球：金属 fuzz=0.0
    auto material_fullmetal = std::make_shared<Metal>(vec3{0.8, 0.8, 0.8}, 0.0);
    // 右边球：金属 fuzz=0.5
    auto material_halfmetal = std::make_shared<Metal>(vec3{0.8, 0.6, 0.2}, 0.5);

    // Scenario Definition
    HittableList world(std::vector<std::shared_ptr<Hittable>> {
        std::make_shared<Plane>(vec3{0, -0.5, 0}, vec3{0, 1, 0}, material_ground),
        std::make_shared<Sphere>(vec3{ 0.0, 0.0, -1.0}, 0.5, material_red),
        std::make_shared<Sphere>(vec3{-1.0, 0.0, -1.0}, 0.5, material_fullmetal),
        std::make_shared<Sphere>(vec3{ 1.0, 0.0, -1.0}, 0.5, material_halfmetal)
    });

    // Camera Definition
    vec3 camera_up = { 0.0, 1.0, 0.0 };

    // Ray Tracing Definition
    const int max_depth = 50;
    const int samples_per_pixel = 200;

    // --- 2. 动画循环 (360 帧) ---
    int total_frames = 360;
    std::cout << "Start rendering with " << omp_get_max_threads() << " threads..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int frame = 0; frame < total_frames; ++frame) {

        // Result: ppm/bin/ffmpeg/frame_00x.ppm
        std::ostringstream ss;
        ss << "ppm/bin/ffmpeg/frame_" << std::setw(3) << std::setfill('0') << frame << ".ppm";
        std::string file_path = ss.str();

        double progress = (double)frame / total_frames;
        
        // 2. 映射到弧度 (0 ~ 2PI)
        double angle_radians = progress * 2.0 * M_PI;

        double radius = 2.0;
        double cam_x = radius * std::sin(angle_radians);
        double cam_z = radius * std::cos(angle_radians);
        double cam_y = 2.0;
        // 圆心在 (0, 2, 0)。
        // x = 0 + r * sin(theta)
        // z = 0 + r * cos(theta)
        // y = 2 (保持高度不变，平行于地面)

        vec3 lookfrom = {cam_x, cam_y, cam_z};

        // C. 更新摄像机
        // 每次循环都用新的位置实例化摄像机
        Camera camera(lookfrom, world.get_object(1)->get_position(), camera_up, 60.0, double(width)/double(height) );

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

        if (frame % 10 == 0) {
            std::cout << "Rendered frame " << frame << " / " << total_frames << "\r" << std::flush;
        }
    }

    std::cout << "\nDone!" << std::endl;
    return 0;
}
