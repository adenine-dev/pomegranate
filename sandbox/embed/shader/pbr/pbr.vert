#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec4 color;

layout(set = 0, binding = 0, std140) uniform Camera {
    mat4 view;
    mat4 projection;
} camera;

layout(set = 1, binding = 0, std140) uniform Model {
    mat4 transform;
} model;

layout(location = 0) out vec4 vertexColor;
layout(location = 1) out vec2 vertexUV;
layout(location = 2) out vec3 vertexNormal;
layout(location = 3) out vec3 worldPos;
layout(location = 4) out vec3 cameraPos;

void main() {
    vertexColor = color;
    vertexUV = uv;
    vertexNormal = mat3(model.transform) * normal;
    worldPos = vec3(model.transform * vec4(position, 1.0));
    cameraPos = inverse(camera.view)[3].xyz;

    gl_Position = camera.projection * camera.view * vec4(worldPos, 1.0);
}