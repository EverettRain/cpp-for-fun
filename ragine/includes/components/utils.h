#pragma once

#include "ragine.h"
#include "stb_image.h"

vec3 gamma_correct(const vec3& origin);
vec3 sampled_gamma(const vec3& origin, int sample_times);
vec3 reflect(const vec3& v, const vec3& n);
vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat);
double reflectance(double cosine, double ref_idx);

struct Colors {
    inline static const vec3 Black   {0.0, 0.0, 0.0};
    inline static const vec3 White   {1.0, 1.0, 1.0};
    inline static const vec3 Red     {1.0, 0.0, 0.0};
    inline static const vec3 Green   {0.0, 1.0, 0.0};
    inline static const vec3 Blue    {0.0, 0.0, 1.0};
    inline static const vec3 Yellow  {1.0, 1.0, 0.0};
    inline static const vec3 Cyan    {0.0, 1.0, 1.0};
    inline static const vec3 Magenta {1.0, 0.0, 1.0};
    
    // 常用灰度
    inline static const vec3 Gray20  {0.2, 0.2, 0.2};
    inline static const vec3 Gray50  {0.5, 0.5, 0.5};
    inline static const vec3 Gray80  {0.8, 0.8, 0.8};
};

class rtw_image {
    const int bytes_per_pixel = 3;
    unsigned char *data;
    int width, height;
    int bytes_per_scanline;
public:
    rtw_image() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

    rtw_image(const char* filename) {
        auto components_per_pixel = bytes_per_pixel;

        data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);

        if (!data) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = width * bytes_per_pixel;
    }

    ~rtw_image() {
        if (data) stbi_image_free(data);
    }

    int get_width() const { return (data == nullptr) ? 0 : width; }
    int get_height() const { return (data == nullptr) ? 0 : height; }

    // 获取像素颜色
    const unsigned char* pixel_data(int x, int y) const {
        static unsigned char magenta[] = { 255, 0, 255 };
        if (data == nullptr) return magenta;

        // 防止坐标越界
        x = std::min(x, width - 1);
        y = std::min(y, height - 1);

        return data + y * bytes_per_scanline + x * bytes_per_pixel;
    }
};