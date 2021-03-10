#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in mat4 model;

uniform vec3 camera_pos;
uniform mat4 view;
uniform mat4 projection;

out vec2 uv;

void main()
{
    vec3 center = vec3(0.0f, 0.0f, 0.0f);
    vec3 viewer = (inverse(model) * vec4(camera_pos, 1.0f)).xyz;
    vec3 normalDir = viewer - center;
    normalDir = normalize(normalDir);

    vec3 upDir = abs(normalDir.y) > 0.999f? vec3(0.0f, 0.0f, 1.0f) : vec3(0.0f, 1.0f, 0.0f);
    vec3 rightDir = normalize(cross(upDir, normalDir));
    upDir = normalize(cross(normalDir, rightDir));

    vec3 centerOffs = pos - center;
    vec3 localPos = center + rightDir * centerOffs.x + upDir * centerOffs.y + normalDir * centerOffs.z;

    gl_Position = projection * view * model * vec4(localPos, 1.0f);
    uv = texcoord;
}