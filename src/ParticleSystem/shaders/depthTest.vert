#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 projPos;
out float d;

vec4 ComputeScreenPos(vec4 p)
{
    vec4 o = p * 0.5;
    o.xy = vec2(o.x + o.w, o.y + o.w);
    o.zw = p.zw;
    return o;
}

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    projPos = ComputeScreenPos(gl_Position);
    d = -(view * model * vec4(aPos, 1.0)).z;
}