#pragma once

#include "ragine.h"
#include "../ray_tracing/object.h"

class bvh_node : public Hittable {
public:
    aabb box;
    std::shared_ptr<Hittable> left_child;
    std::shared_ptr<Hittable> right_child;

    bvh_node() {}

    bvh_node(const std::vector<std::shared_ptr<Hittable>>& src_objects, const size_t start, const size_t end, 
        double time0, double time1) {
        auto objects = src_objects;
        int axis = (int)random_double(0, 3);

        // 提供自定义排序函数，该函数为一个 bool 返回值的匿名函数，传入两个变量
        // 如果返回值为 true，则第一个变量排在第二个变量之前
        // 如果返回值为 false，则第一个变量排在第二个变量之后
        auto comparator = [axis](const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b) {
            aabb box_a, box_b;
            if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b)) 
                std::cerr << "No enough bounding box in bvh_node constructor" << std::endl;
            return box_a.minimum[axis] < box_b.minimum[axis];
        };
        
        size_t object_count = end - start;
        if (object_count == 1) {
            left_child = right_child = objects[start];
        } else if (object_count == 2) {
            if (comparator(objects[start], objects[start + 1])){
                left_child = objects[start];
                right_child = objects[start + 1];
            } else {
                left_child = objects[start + 1];
                right_child = objects[start];
            }
        } else {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);
            size_t mid = start + object_count / 2;
            left_child = std::make_shared<bvh_node>(objects, start, mid, time0, time1);
            right_child = std::make_shared<bvh_node>(objects, mid, end, time0, time1);
        }

        aabb box_left, box_right;
        if (!left_child->bounding_box(0, 0, box_left) || !right_child->bounding_box(0, 0, box_right))
            std::cerr << "No enough bounding box in bvh_node constructor" << std::endl;
        box = surrounding_box(box_left, box_right);
    }

    bvh_node(const HittableList& list, double time0, double time1)
        : bvh_node(list.objects, 0, list.objects.size(), time0, time1) {}

    virtual bool is_hit(const ray& r, hit& record, double t_min, double t_max) const override {
        if (!box.is_hit(r, t_min, t_max)) return false;

        bool hit_left = left_child->is_hit(r, record, t_min, t_max);
        bool hit_right = right_child->is_hit(r, record, t_min, hit_left ? record.time : t_max);

        return hit_left || hit_right;
    }

    virtual bool bounding_box(double t0, double t1, aabb& output_box) const override {
        output_box = box;
        return true;
    }

    virtual vec3 get_position() const override {
        return {0.0, 0.0, 0.0}; 
    }
};