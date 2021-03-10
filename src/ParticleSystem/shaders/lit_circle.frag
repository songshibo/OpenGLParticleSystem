#version 330 core

uniform float radius;
uniform float width_height_ratio;
uniform vec2 center;

in vec2 mPos;

out vec4 FragColor;

void main()
{
    vec2 v = mPos - center; //- vec2(0,0);
    float v_l = pow(v.y, 2)/pow(radius, 2) + pow(v.x, 2)/pow(radius, 2);
    if (v_l > 0.99f && v_l < 1.0f)
    {
        FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        FragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
}