#ifndef SRGB_H
#define SRGB_H

// pow(c, 1./2.2)
vec3 linearToSrgb(vec3 linearRGB) {
    bvec3 cutoff = lessThan(linearRGB, vec3(0.0031308));
    vec3 higher = vec3(1.055) * pow(linearRGB, vec3(1.0/2.4)) - vec3(0.055);
    vec3 lower = linearRGB * vec3(12.92);

    return mix(higher, lower, cutoff);
}

// pow(c, 2.2)
vec3 srgbToLinear(vec3 sRGB) {
    bvec3 cutoff = lessThan(sRGB, vec3(0.04045));
    vec3 higher = pow((sRGB + vec3(0.055))/vec3(1.055), vec3(2.4));
    vec3 lower = sRGB/vec3(12.92);

    return mix(higher, lower, cutoff);
}

float srgbToLinear(float sRGB)
{
    bool cutoff = sRGB < 0.04045;
    float higher = pow((sRGB + 0.055)/1.055, 2.4);
    float lower = sRGB/12.92;

    return mix(higher, lower, cutoff);
}

#endif // SRGB_H
