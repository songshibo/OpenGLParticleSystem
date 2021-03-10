#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 texcoord;

out vec2 uv;

void main()
{
    gl_Position = vec4(aPos.x * 3/5, aPos.y * 3/5, 0.0, 1.0) + vec4(0.6, 0.6, 0.0, 0.0);
    // gl_Position = vec4(aPos.x * 2, aPos.y * 2, 0.0, 1.0);
    uv = texcoord;
}