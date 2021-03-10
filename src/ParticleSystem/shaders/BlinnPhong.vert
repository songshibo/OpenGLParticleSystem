#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec3 world_pos;
out vec3 world_norm;
out vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    world_pos = vec3(model * vec4(vertex, 1.0));
    world_norm = mat3(transpose(inverse(model))) * normal;
    uv = texcoord;

    gl_Position = projection * view * vec4(world_pos, 1.0);
}