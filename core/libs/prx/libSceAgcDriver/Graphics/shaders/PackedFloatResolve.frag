#version 450

layout(set = 0, binding = 0) uniform usampler2DMS source;
layout(push_constant) uniform Constants {
    uint samples;
} constants;
layout(location = 0) out uvec4 color;

float decode(uint bits, uint mantissa) {
    const uint exponent = bits >> mantissa;
    const uint fraction = bits & ((1u << mantissa) - 1u);
    if (exponent == 0u) return ldexp(float(fraction), -14 - int(mantissa));
    return ldexp(1.0 + ldexp(float(fraction), -int(mantissa)), int(exponent) - 15);
}

uint encode(float value, uint mantissa) {
    if (!(value > 0.0)) return 0u;
    const uint bits = floatBitsToUint(value);
    const int exponent = int(bits >> 23) - 127 + 15;
    if (exponent <= 0) return uint(ldexp(value, 14 + int(mantissa)));
    if (exponent >= 31) return 31u << mantissa;
    return (uint(exponent) << mantissa) | ((bits >> (23u - mantissa)) & ((1u << mantissa) - 1u));
}

void main() {
    const ivec2 texel = ivec2(gl_FragCoord.xy);
    float sum[3] = float[3](0.0, 0.0, 0.0);
    uint special[3] = uint[3](0u, 0u, 0u);
    bool hasSpecial[3] = bool[3](false, false, false);
    for (uint sampleIndex = 0u; sampleIndex < constants.samples; ++sampleIndex) {
        const uint word = texelFetch(source, texel, int(sampleIndex)).x;
        const uint fields[3] = uint[3](word & 0x7ffu, (word >> 11u) & 0x7ffu, word >> 22u);
        for (uint field = 0u; field < 3u; ++field) {
            const uint mantissa = field == 2u ? 5u : 6u;
            if ((fields[field] >> mantissa) == 31u) {
                if (!hasSpecial[field]) {
                    hasSpecial[field] = true;
                    special[field] = fields[field];
                }
            } else {
                sum[field] += decode(fields[field], mantissa);
            }
        }
    }
    uint result[3];
    for (uint field = 0u; field < 3u; ++field) {
        const uint mantissa = field == 2u ? 5u : 6u;
        result[field] = hasSpecial[field] ? special[field] : encode(sum[field] / float(constants.samples), mantissa);
    }
    color = uvec4(result[0] | (result[1] << 11u) | (result[2] << 22u), 0u, 0u, 0u);
}
