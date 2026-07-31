#ifndef COLOR_H
#define COLOR_H

#include "rtweekend.h"

#include "interval.h"
#include "vec3.h"

using color = vec3;

// Programs assume that an image is gamma corrected before being written into an image file (0 & 1 values have a transformation applied)
// Linear space (data written w/o being transformed), gamma space (images that are transformed)
// Gamma 2: Take the power from gamma to linear (square)
// Linear -> Gamma: inverse of gamma 2 (square root)
inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0) {
        return std::sqrt(linear_component);
    }

    return 0;
}

void write_color(std::ostream& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Apply a linear to gamma transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Translate the [0,1] component values to the byte range [0,255].
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif