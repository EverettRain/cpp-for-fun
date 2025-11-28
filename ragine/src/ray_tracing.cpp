#include "ragine.h"

vec3 ray_color(const ray& r, const Hittable& world, int depth) {
    if (depth < 1) return {0.0, 0.0, 0.0};

    hit record;
    if (world.is_hit(r, record, MINIMUM, INFINITY)) {
        vec3 attenuation;
        ray out_ray;
        if (record.material->scatter(r, record, attenuation, out_ray)) {
            return ray_color(out_ray, world, depth - 1) * attenuation;
        }
        return {0.0, 0.0, 0.0};
    } 

    vec3 unit_direction = r.dir.normalize();
    double t = 0.5 * (unit_direction.y + 1.0);
    return vec3{1.0, 1.0, 1.0} * (1.0 - t) + vec3{0.5, 0.7, 1.0} * t;
}
