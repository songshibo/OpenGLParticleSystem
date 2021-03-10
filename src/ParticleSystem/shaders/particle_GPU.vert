#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec4 pos_size; // xyz stands for world space position, w stands for size
layout(location = 3) in vec4 ratio_life;

uniform vec3 camera_pos;
uniform mat4 view;
uniform mat4 projection;

out vec2 uv;
out vec4 projPos;
out float d;
out float life_lerp;
out float tex_type;

float zNear = 0.1;
float zFar  = 200.0;

vec4 ComputeScreenPos(vec4 p)
{
    vec4 o = p * 0.5;
    o.xy = vec2(o.x, o.y) + o.w;
    o.zw = p.zw;
    return o;
}

void main()
{
    mat4 model = mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        pos_size.x, pos_size.y, pos_size.z, 1.0f
    );
    mat4 scale = mat4(
        pos_size.w, 0.0f, 0.0f, 0,
        0.0f, pos_size.w, 0.0f, 0,
        0.0f, 0.0f, pos_size.w, 0,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    model = model * scale;
    
    vec3 center = vec3(0.0, 0.0, 0.0);
    // transfer camera position from world space to model space
    vec3 viewer = (inverse(model) * vec4(camera_pos, 1.0)).xyz;
    vec3 normalDir = viewer - center;
    normalDir = normalize(normalDir);

    vec3 upDir = abs(normalDir.y) > 0.999? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
    vec3 rightDir = normalize(cross(upDir, normalDir));
    upDir = normalize(cross(normalDir, rightDir));

    vec3 centerOffs = pos - center;
    vec3 localPos = center + rightDir * centerOffs.x + upDir * centerOffs.y + normalDir * centerOffs.z;

    gl_Position = projection * view * model * vec4(localPos, 1.0);
    // gl_Position = projection * view * model * vec4(pos, 1.0);
    uv = texcoord;
    projPos = ComputeScreenPos(gl_Position);
    d = -(view * model * vec4(localPos, 1.0)).z;
    life_lerp = ratio_life.x;
    tex_type = ratio_life.y;
}