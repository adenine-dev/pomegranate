#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

layout(set = 0, binding = 0, std140) uniform ModelViewProjection {
    mat4 model;
    mat4 view;
    mat4 projection;
} mvp;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = mvp.projection * mvp.view * mvp.model * vec4(position, 1.0);

    fragColor = color;
}