#pragma once

#include "ragine.h"

class Hittable_legend {
public:
    virtual bool is_hit(const ray& r, hit_legend& record, double t_min, double t_max) const = 0;
};

class HittableList_legend : public Hittable_legend {
    std::vector<std::shared_ptr<Hittable_legend>> objects;

public:
    void add(std::shared_ptr<Hittable_legend> object) { objects.push_back(object); }
    virtual bool is_hit(const ray& r, hit_legend& record, double t_min, double t_max) const override {
        hit_legend temp;
        bool hit_anything = false;
        double closet_so_far = t_max;

        for (const auto& object: objects) {
            if (object->is_hit(r, temp, t_min, closet_so_far)) {
                hit_anything = true;
                closet_so_far = temp.time;
                record = temp;
            }
        }

        return hit_anything;
    }
};

class Sphere_legend : public Hittable_legend {
public:
    vec3 center;
    double radius;
    vec3 color;

    /// @brief Instantiate a Sphere_legend Instance
    /// @param cent The center of Sphere_legend
    /// @param r The radius of Sphere_legend
    /// @param col The color of Sphere_legend
    Sphere_legend(const vec3& cent, const double r, const vec3& col) : center(cent), radius(r), color(col) {}

    virtual bool is_hit(const ray& r, hit_legend& record, double t_min, double t_max) const override {
        vec3 co = r.origin - center;
        double a = r.dir.dot(r.dir);
        double b = 2.0 * co.dot(r.dir);
        double c = co.dot(co) - radius * radius;
        double discriminant = b * b - 4 * a * c;

        if (discriminant < 0) return false;

        double sqrtd = std::sqrt(discriminant);

        double root = (-b - sqrtd) / (2.0 * a);

        if (root < std::max(MINIMUM, t_min) || root > t_max) {
            root = (-b + sqrtd) / (2.0 * a);
            if (root < std::min(0.001, t_min) || root > t_max) return false;
        }

        record.time = root;
        record.position = r.origin + r.dir * root;
        record.normal = (record.position - center) * (1.0 / radius);
        record.color = this->color;

        return true;
    }
};

class Plane_legend : public Hittable_legend {
public:
    vec3 locate;
    vec3 normal;
    vec3 color;

    /// @brief Instantiate a Plane_legend Instance
    /// @param point Position of any point on Plane_legend
    /// @param n The normal direction of Plane_legend
    /// @param col The color of Plane_legend
    Plane_legend(const vec3& point, const vec3& n, const vec3& col) : locate(point), normal(n), color(col) {}

    virtual bool is_hit(const ray& r, hit_legend& record, double t_min, double t_max) const override {
        // 几何逻辑：
        // 射线 P(t) = A + tb
        // 代入平面方程：(A + tb - P0) · N = 0
        // 解出 t = (P0 - A) · N / (b · N)
        double denominator = r.dir.dot(normal);

        if (std::abs(denominator) < 1e-6) return false;

        double root = (locate - r.origin).dot(normal) / denominator;

        if (root < std::max(MINIMUM, t_min) || root > t_max) return false;

        record.time = root;
        record.position = r.at(root);
        record.normal = normal;
        record.color = this->color;

        return true;
    }
};
