#version 330 core

layout (location = 0) in vec3 pos;

out vec2 mPos;

void main()
{
    mPos = vec2(pos.x, pos.y);
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
}