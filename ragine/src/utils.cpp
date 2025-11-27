#include "ragine.h"

vec3 gamma_correct(const vec3& origin) {
    double r = sqrt(origin.x / 255.0) * 255.0;
    double g = sqrt(origin.y / 255.0) * 255.0;
    double b = sqrt(origin.z / 255.0) * 255.0;
    return {r, g, b};
}
