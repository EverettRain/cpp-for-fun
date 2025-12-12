#pragma once

#include "ragine.h"

class Material {
public:
    virtual bool scatter(const ray& ray_in, const hit& record, vec3& attenuation, ray& ray_out) const = 0;
};

class Lambertian : public Material {
    // vec3 albedo;
    std::shared_ptr<Texture> albedo;
public:
    Lambertian(const vec3& color) : albedo(std::make_shared<SolidColor>(color)) {}
    Lambertian(std::shared_ptr<Texture> alb) : albedo(alb) {}
    virtual bool scatter(const ray& ray_in, const hit& record, vec3& attenuation, ray& ray_out) const override {
        ray new_ray;
        new_ray.origin = record.position;
        new_ray.dir = (record.normal + random_unit_vector());

        if (new_ray.dir.length() < 1e-8) new_ray.dir = record.normal;
        new_ray.dir = new_ray.dir.normalize();
        attenuation = albedo->value(record.uv, record.position);
        ray_out = new_ray;
        return true;
    }
};

class Metal : public Material {
    vec3 albedo;
    double fuzz;
public:
    Metal(const vec3& color, double fuz) : albedo(color), fuzz(fuz < 1.0 ? (fuz > 0.0 ? fuz : 0.0) : 1.0) {}
    virtual bool scatter(const ray& ray_in, const hit& record, vec3& attenuation, ray& ray_out) const override {
        ray new_ray;
        new_ray.origin = record.position;
        new_ray.dir = reflect(ray_in.dir.normalize(), record.normal);
        vec3 fuzz_fix = random_unit_vector() * fuzz;
        
        new_ray.dir = (new_ray.dir + fuzz_fix).normalize();
        attenuation = albedo;
        ray_out = new_ray;
        return (ray_out.dir.dot(record.normal) > 0);
    }
};

class Dielectric : public Material {
    vec3 albedo;
    double ir;
public:
    Dielectric(const vec3& color, double ire) : albedo(color), ir(ire) {}
    virtual bool scatter(const ray& ray_in, const hit& record, vec3& attenuation, ray& ray_out) const override {
        attenuation = albedo;
        double refraction_ratio = ray_in.dir.dot(record.normal) > 0 ? ir : (1 / ir);
        vec3 unit_direction = ray_in.dir.normalize();

        vec3 correct_normal = ray_in.dir.dot(record.normal) > 0 ? record.normal * (-1.0) : record.normal;
        double cos_theta = std::min((unit_direction * -1.0).dot(correct_normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
            direction = reflect(unit_direction, correct_normal);
        } else {
            direction = refract(unit_direction, correct_normal, refraction_ratio);
        }
        
        ray_out = {record.position, direction};
        return true;
    }
};