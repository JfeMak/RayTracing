#ifndef VEC3_H
#define VEC3_H

#include "rtweekend.h"

class vec3 {
  public:
    double e[3];

    vec3() : e{0,0,0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t) {
        return *this *= 1/t;
    }

    double length() const {
        return std::sqrt(length_squared());
    }

    double length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    bool near_zero() const {
        // Return true if the vector is close to zero in all dimensions.
        auto s = 1e-8;
        return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
    }

    static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max) {
        return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
    }
};

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using point3 = vec3;


// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_unit_vector() {
    while (true) {
        auto p = vec3::random(-1, 1);
        auto lensq = p.length_squared();

        // Very small value squared causes norm to be roughly 0, so normalized vector will become (+- inf, +- inf, +- inf)
        // Only accept values > 1e-160 (64 bit floats support that precision)
        if (1e-160 < lensq && lensq <= 1) {
            return p / sqrt(lensq);
        }
    }
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) { // In the same hemisphere as the normal
        return on_unit_sphere;
    } else {
        return -on_unit_sphere;
    }
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    // v is ray, n is normal -> reflected ray direction is v + 2b where b is just the normal vector scaled by dot(v, n)
    // dot(v, n) is negative though, so dot(v, n) * n goes opposite of n, when it should go in same direction -> negate
    return v - 2 * dot(v, n) * n;
}

/**
 * Proof using Snell's Law:
 * 
 * - R  : Incoming ray unit vector (pointing into the hit surface)
 * - n  : Surface normal unit vector (pointing out of the surface toward R)
 * - R' : Refracted ray unit vector (pointing into the second medium)
 * - theta  : Incident angle between -R and n  => cos(theta) = -R . n
 * - theta' : Refracted angle between -R' and n => cos(theta') = -R' . n
 * - eta, eta' : Refractive indices for medium 1 (incident) and medium 2 (refracted)
 * 
 * * We decompose the refracted ray R' into perpendicular and parallel components:
 * R' = R'_perp + R'_parallel
 * 
 * R'_perp
 * 1. Decompose incoming unit ray R relative to normal n:
 * R = R_parallel + R_perp
 * * 2. Express R_parallel (points in direction of -n with magnitude cos(theta)):
 * R_parallel = -cos(theta) * n
 * * 3. Isolate R_perp:
 * R_perp = R - R_parallel
 * R_perp = R - (-cos(theta) * n)
 * R_perp = R + cos(theta) * n
 * * 4. Find the magnitude of R_perp (from right-triangle geometry, |R| = 1):
 * |R_perp| = sin(theta)
 * * 5. Define horizontal unit vector u_hat along the surface boundary:
 * u_hat = R_perp / |R_perp|
 * u_hat = R_perp / sin(theta)
 * * 6. Express R'_perp using its magnitude sin(theta') and direction u_hat:
 * R'_perp = sin(theta') * u_hat
 * R'_perp = sin(theta') * (R_perp / sin(theta))
 * R'_perp = (sin(theta') / sin(theta)) * R_perp
 * * 7. Rearrange Snell's Law (eta * sin(theta) = eta' * sin(theta')):
 * sin(theta') / sin(theta) = eta / eta'
 * * 8. Substitute the Snell ratio and R_perp (from step 3) into R'_perp:
 * R'_perp = (eta / eta') * (R + cos(theta) * n)
 * 
 * R'_parallel:
 * 1. Since R' is a unit vector (|R'| = 1), use the Pythagorean theorem:
 * |R'|^2 = |R'_perp|^2 + |R'_parallel|^2 = 1
 * * 2. Isolate magnitude |R'_parallel|:
 * |R'_parallel|^2 = 1 - |R'_perp|^2
 * |R'_parallel|   = sqrt(1 - |R'_perp|^2)
 * * 3. Direction of R'_parallel goes down into the second medium (-n):
 * R'_parallel = -sqrt(1 - |R'_perp|^2) * n
 * 
 * Result:
 * R' = R'_perp + R'_parallel
 * R' = (eta / eta') * (R + cos(theta) * n) - sqrt(1 - |R'_perp|^2) * n
 */
inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = std::fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

#endif