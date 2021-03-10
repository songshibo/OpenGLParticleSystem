#version 330 core

in vec2 uv;
out vec4 color;

uniform sampler2D MainTex;
uniform float life_lerp_factor;

void main()
{
    vec4 albedo = texture(MainTex, uv);
    if (albedo.a < 0.2f) // in case that draw too many transparent pixels
        discard;
    color = vec4(albedo.rgb, albedo.a * life_lerp_factor * 0.5);
}