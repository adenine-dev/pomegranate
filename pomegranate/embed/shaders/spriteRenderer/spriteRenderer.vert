#version 450

layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform PushConstants {
    mat4 viewProjection;
} constants;

struct QuadData {
    mat4 transform;
    vec4 color;
};

const vec4 QUAD_VERTICIES[6] = vec4[] (
    vec4( 0.5, 0,  0.5, 1), vec4(-0.5, 0, -0.5, 1), vec4(-0.5, 0,  0.5, 1),
    vec4(-0.5, 0, -0.5, 1), vec4( 0.5, 0,  0.5, 1), vec4( 0.5, 0, -0.5, 1)
);


layout(set = 0, binding = 0, std140) uniform UBO {
    QuadData data[10000];
} ubo;

void main() {
    gl_Position = constants.viewProjection * ubo.data[gl_InstanceIndex].transform * QUAD_VERTICIES[gl_VertexIndex];
    fragColor = ubo.data[gl_InstanceIndex].color;
}