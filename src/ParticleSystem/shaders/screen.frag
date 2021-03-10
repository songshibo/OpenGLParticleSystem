#version 330 core
in vec2 uv;
out vec4 FragColor;

uniform sampler2D depth;
float zNear = 0.1;
float zFar  = 100.0;

float LinearizeDepth(in vec2 coords)
{
    float depth = texture(depth, coords).r;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
    FragColor = vec4(vec3(LinearizeDepth(uv)), 1.0);
}