#version 450
// Full-screen pass: read the HDR scene colour, apply exposure and the ACES
// curve, and write the result. The swap chain image is an sRGB format,
// so the GPU applies the sRGB encoding when the shader writes to it.

layout(set = 0, binding = 0) uniform sampler2D hdrScene;
layout(push_constant) uniform Params {
    float exposureStops;
} params;

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 outColour;

vec3 acesFitted(vec3 x) {
    const float a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec3 hdr = texture(hdrScene, uv).rgb * exp2(params.exposureStops);
    outColour = vec4(acesFitted(hdr), 1.0);
}
