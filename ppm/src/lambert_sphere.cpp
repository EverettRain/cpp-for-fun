#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

// --- 1. 基础向量数学类 (Vec3) ---
// 图形学离不开向量，为了代码整洁，我们定义一个简单的类
struct Vec3 {
    double x, y, z;

    Vec3 operator+(const Vec3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vec3 operator-(const Vec3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vec3 operator*(double t) const { return {x * t, y * t, z * t}; }

    // 点积 (Dot Product)：计算光照的核心
    double dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }

    // 归一化 (Normalize)：把向量长度变为 1，方便计算方向
    Vec3 normalize() const {
        double len = std::sqrt(x * x + y * y + z * z);
        return {x / len, y / len, z / len};
    }
};

// --- 2. 核心功能函数 ---

// 计算射线是否击中球体
// center: 球心, radius: 半径, origin: 射线起点, dir: 射线方向
// 返回值: > 0 表示击中时的距离 t，-1.0 表示没击中
double hit_sphere(const Vec3& center, double radius, const Vec3& origin, const Vec3& dir) {
    // 这里利用了球体方程：(P - C) * (P - C) = r^2
    // 这是一个一元二次方程求解 (at^2 + bt + c = 0)
    Vec3 oc = origin - center;
    double a = dir.dot(dir);
    double b = 2.0 * oc.dot(dir);
    double c = oc.dot(oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-b - std::sqrt(discriminant)) / (2.0 * a);
    }
}

int main() {
    const int width = 800;
    const int height = 600;

    std::ofstream ofs("sphere_lambert.ppm", std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";

    // 场景定义
    Vec3 lower_left_corner = {-2.0, -1.5, -1.0}; // 屏幕左下角在 3D 空间的位置
    Vec3 horizontal = {4.0, 0.0, 0.0}; // 屏幕宽度向量
    Vec3 vertical = {0.0, 3.0, 0.0}; // 屏幕高度向量
    Vec3 origin = {0.0, 0.0, 0.0}; // 摄像机(眼睛)位置

    Vec3 sphere_center = {0.0, 0.0, -1.0}; // 球放在屏幕后面 z = -1
    double sphere_radius = 0.5;

    // 定义一个光照方向 (光从右上方打过来)
    Vec3 light_dir = Vec3{1.0, 1.0, 1.0}.normalize();

    for (int j = height - 1; j >= 0; --j) {
        for (int i = 0; i < width; ++i) {

            // 1. 计算当前像素对应的 UV 坐标 (0.0 ~ 1.0)
            double u = double(i) / (width - 1);
            double v = double(j) / (height - 1);

            // 2. 生成射线方向
            // 方向 = 左下角 + u*宽 + v*高 - 原点
            Vec3 ray_dir = lower_left_corner + horizontal * u + vertical * v - origin;

            // 3. 检测碰撞
            double t = hit_sphere(sphere_center, sphere_radius, origin, ray_dir);

            if (t > 0.0) {
                // --- 击中球体，开始计算光照 ---

                // 计算撞击点坐标 P = origin + t * dir
                Vec3 hit_point = origin + ray_dir * t;

                // 计算法线 (Normal)：球面上某点的法线就是 (点 - 球心)
                Vec3 normal = (hit_point - sphere_center).normalize();

                // Lambert 光照计算 (Diffuse)
                // 亮度 = max(0, 法线 · 光照方向)
                // 点积为 1 表示正对光源(最亮)，为 0 表示垂直(黑)，负数表示背光
                double diff = std::max(0.0, normal.dot(light_dir));

                // 基础颜色
                int r = static_cast<int>(50 * diff);
                int g = static_cast<int>(255 * diff);
                int b = static_cast<int>(50 * diff);

                ofs << (unsigned char)r << (unsigned char)g << (unsigned char)b;
            } else {
                // --- 没击中，画背景 (简单的渐变色) ---
                Vec3 unit_direction = ray_dir.normalize();
                double t = 0.5 * (unit_direction.y + 1.0);
                // 混合白色和蓝色
                unsigned char r = static_cast<unsigned char>(255 * (1.0 - t) + 0.5 * 255 * t);
                unsigned char g = static_cast<unsigned char>(255 * (1.0 - t) + 0.7 * 255 * t);
                unsigned char b = static_cast<unsigned char>(255 * (1.0 - t) + 1.0 * 255 * t);
                ofs << r << g << b;
            }
        }
    }

    ofs.close();
    std::cout << "Done! Generated sphere_lambert.ppm" << std::endl;
    return 0;
}
