#version 450

layout(location = 0) in vec3 fragCoord;

layout (set = 0, binding = 1) uniform samplerCube skybox;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PC {
    uint mip;
} constants;

void main() {
    outColor = textureLod(skybox, fragCoord, int(constants.mip));
    gl_FragDepth = 1;
}