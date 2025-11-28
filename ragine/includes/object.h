#pragma once

#include "ragine.h"

class Hittable {
public:
    virtual bool is_hit(const ray& r, hit& record, double t_min, double t_max) const = 0;
    virtual vec3 get_position() const = 0;
};

class HittableList : public Hittable {
    std::vector<std::shared_ptr<Hittable>> objects;

public:
    HittableList() {}
    HittableList(std::vector<std::shared_ptr<Hittable>> i_objects) : objects(std::move(i_objects)) {}
    void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }
    virtual bool is_hit(const ray& r, hit& record, double t_min, double t_max) const override {
        hit temp;
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
    virtual vec3 get_position() const override {
        return {0.0, 0.0, 0.0};
    }

    std::shared_ptr<Hittable> get_object(int index) {
        return objects[index];
    }
};

class Sphere : public Hittable {
public:
    vec3 center;
    double radius;
    std::shared_ptr<Material> material;

    /// @brief Instantiate a Sphere Instance
    /// @param cent The center of Sphere
    /// @param r The radius of Sphere
    /// @param mat The material of Sphere
    Sphere(const vec3& cent, const double r, const std::shared_ptr<Material>& mat) : 
        center(cent), radius(r), material(mat) {}

    virtual vec3 get_position() const override {
        return center;
    }

    virtual bool is_hit(const ray& r, hit& record, double t_min, double t_max) const override {
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
        record.material = material.get();;

        return true;
    }
};

class Plane : public Hittable {
public:
    vec3 locate;
    vec3 normal;
    std::shared_ptr<Material> material;

    /// @brief Instantiate a Plane Instance
    /// @param point Position of any point on Plane
    /// @param n The normal direction of Plane
    /// @param mat The material of Plane
    Plane(const vec3& point, const vec3& n, const std::shared_ptr<Material>& mat) : 
        locate(point), normal(n), material(mat) {}

    virtual vec3 get_position() const override {
        return locate;
    }

    virtual bool is_hit(const ray& r, hit& record, double t_min, double t_max) const override {
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
        record.material = material.get();

        return true;
    }
};
