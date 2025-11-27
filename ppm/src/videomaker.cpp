#include "ragine.h"
#include <sstream> // 必须引入：用于构建字符串流
#include <iomanip> // 必须引入：用于设置填充 (001)

int main() {
    const int width = 800;
    const int height = 600;
    
    // --- 1. 场景定义 (移到循环外，因为场景本身是不动的) ---
    HittableList world;
    vec3 sphere_pos1 = {0, 0, -1};
    vec3 sphere_pos2 = {1, 0, 1};
    
    // 创建物体
    Sphere sphere1(sphere_pos1, 0.5, {220.0, 220.0, 220.0});
    Sphere sphere2(sphere_pos2, 0.5, {220.0, 220.0, 220.0});
    Plane ground({0, -2, 0}, {0, 1, 0}, {100.0, 200.0, 100.0});
    
    // 添加到世界
    world.add(std::make_shared<Sphere>(sphere1));
    world.add(std::make_shared<Sphere>(sphere2));
    world.add(std::make_shared<Plane>(ground));

    // 光源定义
    vec3 light_pos = {200.0, 200.0, 100.0};
    double shininess = 32;

    // --- 2. 动画循环 (360 帧) ---
    int total_frames = 360;
    std::cout << "Start rendering animation..." << std::endl;

    for (int frame = 0; frame < total_frames; ++frame) {
        
        // A. 生成文件名 (C++ 风格，安全)
        // 结果类似: ppm/bin/frame_000.ppm, ppm/bin/frame_001.ppm
        std::ostringstream ss;
        ss << "ppm/bin/video/frame_" << std::setw(3) << std::setfill('0') << frame << ".ppm";
        std::string file_path = ss.str();

        std::ofstream ofs(file_path, std::ios::binary);
        if (!ofs) {
            std::cerr << "Failed to create file: " << file_path << std::endl;
            return 1;
        }
        ofs << "P6\n" << width << " " << height << "\n255\n";

        // B. 计算摄像机位置 (Orbital Movement)
        // 目标：绕 (0,4,0) 旋转，半径为 4
        double angle_degrees = (double)frame; // 0 ~ 359 度
        double angle_radians = angle_degrees * M_PI / 180.0;

        double radius = 4.0;
        // 圆心在 (0, 4, 0)。
        // x = 0 + r * sin(theta)
        // z = 0 + r * cos(theta)
        // y = 4 (保持高度不变，平行于地面)
        double cam_x = radius * std::sin(angle_radians);
        double cam_z = radius * std::cos(angle_radians);
        double cam_y = 4.0; 

        vec3 lookfrom = {cam_x, cam_y, cam_z};
        
        // 摄像机盯着场景中心 (比如 sphere_pos1 或者原点)
        vec3 lookat = {0, 0, 0}; 
        vec3 vup = {0, 1, 0};

        // C. 更新摄像机
        // 注意：每次循环都要用新的位置实例化摄像机
        Camera camera(lookfrom, lookat, vup, 60.0, double(width)/double(height));

        // D. 像素渲染循环
        for (int y = height - 1; y > -1; y--) {
            for (int x = 0; x < width; x++) {
                double u = (double)x / (width - 1);
                double v = (double)y / (height - 1);

                ray r = camera.get_ray(u, v);
                hit rec;

                // 注意：这里使用 MINIMUM (比如 0.001) 防止自遮挡
                if (world.is_hit(r, rec, MINIMUM, INFINITY)) {
                    vec3 color;
                    vec3 light_dir = (light_pos - rec.position).normalize();

                    // --- 阴影逻辑 ---
                    ray shadow_ray;
                    // 重要优化：起点稍微偏移，防止 "Shadow Acne" (黑麻子)
                    // 如果你的 MINIMUM 已经在 is_hit 里处理了，这里 rec.position 也没问题
                    // 但标准做法是 rec.position + rec.normal * 1e-4
                    shadow_ray.origin = rec.position; 
                    shadow_ray.dir = light_dir;

                    hit shadow_rec;
                    double light_distance = (light_pos - rec.position).length();
                    
                    // 检查是否在阴影中
                    bool in_shadow = world.is_hit(shadow_ray, shadow_rec, MINIMUM, light_distance);

                    if (in_shadow) {
                        color = rec.color * 0.02; // 环境光 (调亮一点点防止死黑)
                    } else {
                        // 使用你实现的 Blinn-Phong
                        // 注意：Blinn-Phong 需要传入的是摄像机位置 (camera.origin 或 lookfrom)
                        color = blinn_phong_shader(rec, light_pos, camera.position(), rec.color, shininess);
                    }

                    // Gamma Correction
                    // 假设 gamma_correct 函数你已经实现 (开根号)
                    color = gamma_correct(color);

                    ofs << (unsigned char)std::min(255.0, color.x) 
                        << (unsigned char)std::min(255.0, color.y)
                        << (unsigned char)std::min(255.0, color.z);
                } else {
                    // 背景
                    vec3 unit_direction = r.dir.normalize();
                    double t = 0.5 * (unit_direction.y + 1.0);
                    unsigned char r_bg = static_cast<unsigned char>(255 * (1.0 - t) + 0.5 * 255 * t);
                    unsigned char g_bg = static_cast<unsigned char>(255 * (1.0 - t) + 0.7 * 255 * t);
                    unsigned char b_bg = static_cast<unsigned char>(255 * (1.0 - t) + 1.0 * 255 * t);
                    ofs << r_bg << g_bg << b_bg;
                }
            }
        }
        
        ofs.close(); // 这一帧写完了，关闭文件
        
        // 可选：打印进度
        if (frame % 10 == 0) {
            std::cout << "Rendered frame " << frame << " / " << total_frames << "\r" << std::flush;
        }
    }

    std::cout << "\nDone!" << std::endl;
    return 0;
}