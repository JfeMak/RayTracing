#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "interval.h"
#include "vec3.h"

class sphere : public hittable {
    public:
        // Stationary Sphere
        sphere(const point3& static_center, double radius, shared_ptr<material> mat) 
            : center(static_center, vec3(0, 0, 0)), radius(std::fmax(0, radius)), mat(mat) {
            auto rvec = vec3(radius, radius, radius);
            bbox = aabb(static_center - rvec, static_center + rvec);
        }

        // Moving Sphere
        sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat)
            : center(center1, center2 - center1), radius(std::fmax(0, radius)), mat(mat) {
            auto rvec = vec3(radius, radius, radius);
            aabb box1(center.at(0) - rvec, center.at(0) + rvec);
            aabb box2(center.at(1) - rvec, center.at(1) + rvec);
            bbox = aabb(box1, box2);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            /*
            Sphere defined as (C_x − x)^2 + (C_y − y)^2 + (C^z − z)^2 = r^2
            Define point P, center C
            dot(C - P, C - P) = (C_x − x)^2 + (C_y − y)^2 + (C^z − z)^2 = r^2

            Want to know if ray Q + td hits sphere
            dot(C - (Q + td), C - (Q + td)) = r^2
            dot(-td + (C - Q), -td + (C - Q)) = r^2

            Dot prod rules
            t^2 dot(d, d) - 2t dot(d, C - Q) + dot(C - Q, C - Q) - r^2 = 0

            Quad Formula
            a = dot(d, d)
            b = -2 dot(d, C - Q)
            c = dot(C - Q, C - Q) - r^2
            oc = C - Q for convenience

            0 roots = no intersection, 1 root = tangent, 2 roots = non-tangent intersection

            Simplification (Notice how b has a factor of -2)
            b = -2h
            h = b / (-2) = dot(d, C - Q)
            */
            point3 current_center = center.at(r.time());
            vec3 oc = current_center - r.origin();
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius*radius;

            auto discriminant = h*h - a*c;
            if (discriminant < 0)
                return false;

            auto sqrtd = std::sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            auto root = (h - sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                root = (h + sqrtd) / a;
                if (!ray_t.surrounds(root))
                    return false;
            }

            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - current_center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat = mat;

            return true;
        }
        
        aabb bounding_box() const override { 
            return bbox;
        }

    private:
        ray center;
        double radius;
        shared_ptr<material> mat;
        aabb bbox;
};

#endif