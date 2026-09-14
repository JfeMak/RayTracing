#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "hittable.h"
#include "texture.h"

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
        lambertian(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}
        lambertian(shared_ptr<texture> tex) : tex(tex) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            auto scatter_direction = rec.normal + random_unit_vector();

            // Catch degenerate scatter direction (directly opposite normal; causes zero scatter direction vector -> infs & NaNs)
            if (scatter_direction.near_zero()) {
                scatter_direction = rec.normal;
            }

            scattered = ray(rec.p, scatter_direction, r_in.time());
            attenuation = tex->value(rec.u, rec.v, rec.p);
            return true;
        }
    private:
        shared_ptr<texture> tex;
};

class metal : public material {
    public:
        metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            vec3 reflected = reflect(r_in.direction(), rec.normal);

            // Fuzz sphere only makes sense if it's scaled compared to the reflection vector -> normalize reflected ray
            reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
            scattered = ray(rec.p, reflected, r_in.time());
            attenuation = albedo;

            // For big spheres (large fuzz values), we might scatter below the surface -> absorb
            return (dot(scattered.direction(), rec.normal) > 0);
        }
    
    private:
        color albedo;
        double fuzz;
};

class dielectric : public material {
    public:
        dielectric(double refraction_index) : refraction_index(refraction_index) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            attenuation = color(1.0, 1.0, 1.0);
            double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

            vec3 unit_direction = unit_vector(r_in.direction());

            // Case where a ray enters a medium of lower index of refraction at a glancing-ish angle -> refract w/ an angle > 90
            // Ex. eta = 1.5, eta' = 1.0, by snell's law, sin theta' = 1.5/1.0 * sin theta 
            // -> 1.5 sin theta / 1.0 > 1.0 (Equality broken since sin theta ' cannot be greater than 1)
            // If sol doesn't exist, then the glass must reflect the ray rather than refract
            double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

            bool cannot_refract = ri * sin_theta > 1.0;
            vec3 direction;

            if (cannot_refract || reflectance(cos_theta, ri) > random_double())
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, ri);

            scattered = ray(rec.p, direction, r_in.time());
            return true;
        }

    private:
        // Refractive index in vacuum or air, or the ratio of the material's refractive index over
        // the refractive index of the enclosing media
        double refraction_index;

        static double reflectance(double cosine, double refraction_index) {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1 - refraction_index) / (1 + refraction_index);
            r0 = r0 * r0;
            return r0 + (1 - r0) * std::pow((1 - cosine), 5);
        }
};

#endif