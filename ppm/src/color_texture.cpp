#include <fstream>
#include <iostream>
#include <vector>

int main() {
    const char* output_path = "output_texture.ppm";
    const int width = 16 * 60;
    const int height = 9 * 60;

    std::ofstream ofs("output_cpp.ppm", std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char r = (unsigned char)((float)x / width * 255.0f);
            unsigned char g = (unsigned char)((float)y / height * 255.0f);
            unsigned char b = 50;

            ofs << r << g << b;
        }
    }

    ofs.close();
    std::cout << "PPM File Generated: " << output_path << std::endl;

    return 0;
}
