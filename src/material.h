#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "hittable.h"

class material {
  public:
    virtual ~material() = default;

    // Returns false if ray is completely absorbed (return black)
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
        return false;
    }
};

// Non-uniform Lambertian distribution (more likely to scatter in surface normal's direction)
class lambertian : public material {
    public:
        lambertian(const color& albedo) : albedo(albedo) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            auto scatter_direction = rec.normal + random_unit_vector();

            // Catch degenerate scatter direction (directly opposite normal; causes zero scatter direction vector -> infs & NaNs)
            if (scatter_direction.near_zero()) {
                scatter_direction = rec.normal;
            }

            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }
    private:
        color albedo;
};

class metal : public material {
    public:
        metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            vec3 reflected = reflect(r_in.direction(), rec.normal);

            // Fuzz sphere only makes sense if it's scaled compared to the reflection vector -> normalize reflected ray
            reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
            scattered = ray(rec.p, reflected);
            attenuation = albedo;

            // For big spheres (large fuzz values), we might scatter below the surface -> absorb
            return (dot(scattered.direction(), rec.normal) > 0);
        }
    
    private:
        color albedo;
        double fuzz;
};

#endif