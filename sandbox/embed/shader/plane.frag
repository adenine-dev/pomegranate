// adapted from
// http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid

#version 450

layout(location = 1) in vec3 nearPoint;
layout(location = 2) in vec3 farPoint;
layout(location = 3) in mat4 fragView;
layout(location = 7) in mat4 fragProj;

layout(location = 0) out vec4 outColor;

const float near = 0.01;
const float far = 150.0;

layout(set = 0, binding = 1) uniform GridConfig {
    vec4 xColor;
    vec4 zColor;
    vec4 gridColor;
    float majorLineScale;
    float minorLineScale;
    float fadeStrength;
    float axisIntensity;
} config;

vec4 grid(vec3 fragPos3D, float scale) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minz = min(derivative.y, 2);
    float minx = min(derivative.x, 2);
    vec4 color = config.gridColor;
    color.w -=min(line, 1.0);

    if(fragPos3D.x > -0.1 * minx && fragPos3D.x < 0.1 * minx) color = config.zColor;
    if(fragPos3D.z > -0.1 * minz && fragPos3D.z < 0.1 * minz) color = config.xColor;
    
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos) {
    float clip_space_depth = computeDepth(pos) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between near and far
    return linearDepth / far; // normalize
}

void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (config.fadeStrength - linearDepth));

    outColor = (grid(fragPos3D, config.majorLineScale) + grid(fragPos3D, config.minorLineScale)) * float(t > 0);

    outColor.a *= fading;
}
