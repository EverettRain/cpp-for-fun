#pragma once
#include "ragine.h"

class Texture {
public:
    virtual vec3 value(const vec2& uv, const vec3& position) const = 0;
};

class SolidColor : public Texture {
    vec3 color_value;

public:
    SolidColor(const vec3& c) : color_value(c) {}
    SolidColor(const double r, const double g, const double b) : color_value(vec3{r, g, b}) {}

    virtual vec3 value(const vec2& uv, const vec3& position) const override { 
        return color_value; 
    }
};

class CheckerTexture : public Texture {
public:
    std::shared_ptr<Texture> odd_color;
    std::shared_ptr<Texture> even_color;
    double scale;

    CheckerTexture(std::shared_ptr<Texture> color1, std::shared_ptr<Texture> color2, double sc = 10.0) :
        even_color(color1), odd_color(color2), scale(sc) {}
    CheckerTexture(const vec3& color1, const vec3& color2, double sc = 10.0) :
        odd_color(std::make_shared<SolidColor>(color1)), even_color(std::make_shared<SolidColor>(color2)), scale(sc) {}
    
    virtual vec3 value(const vec2& uv, const vec3& position) const override { 
        int u_int = static_cast<int>(std::floor(scale * uv.x));
        int v_int = static_cast<int>(std::floor(scale * uv.y));
        bool is_even = (u_int + v_int) % 2 == 0;

        return is_even ? even_color->value(uv, position) : odd_color->value(uv, position);
    }
};

class ImageTexture : public Texture {
private:
    rtw_image image;
    double scale_u, scale_v;   // 缩放 (Tiling): >1 变密, <1 变大
    double offset_u, offset_v; // 偏移: 0~1 移动纹理位置

public:
    ImageTexture(const char* filename) 
        : image(filename), scale_u(1.0), scale_v(1.0), offset_u(0.0), offset_v(0.0) {
            std::cout << "Loaded texture: " << filename << " (" << image.get_width() << "x" << image.get_height() << ")" << std::endl;
        }

    ImageTexture(const char* filename, const vec2& scale, const vec2& offset)
        : image(filename), scale_u(scale.x), scale_v(scale.y), offset_u(offset.x), offset_v(offset.y) {
             std::cout << "Loaded texture: " << filename << "..." << std::endl;
        }

    virtual vec3 value(const vec2& uv, const vec3& position) const override {
        if (image.get_height() <= 0) return vec3{0, 1, 1};

        // 1. 应用变换 (Scale & Offset)
        double u_trans = uv.x * scale_u + offset_u;
        double v_trans = uv.y * scale_v + offset_v;

        // 2. 处理平铺 (Wrapping/Repeat)
        // 使用 fmod 取余数，并处理负数情况，确保坐标永远在 [0, 1) 之间
        u_trans = u_trans - std::floor(u_trans);
        v_trans = v_trans - std::floor(v_trans);

        // 3. 翻转 V 轴
        v_trans = 1.0 - v_trans;

        // 4. 映射到像素坐标
        auto i = static_cast<int>(u_trans * image.get_width());
        auto j = static_cast<int>(v_trans * image.get_height());

        if (i >= image.get_width())  i = image.get_width() - 1;
        if (j >= image.get_height()) j = image.get_height() - 1;

        const auto pixel = image.pixel_data(i, j);
        auto color_scale = 1.0 / 255.0;

        return vec3{
            color_scale * pixel[0],
            color_scale * pixel[1],
            color_scale * pixel[2]
        };
    }
};