#version 330 core

in vec2 uv;
in vec4 projPos;
in float d;
in float life_lerp;
in float tex_type;
out vec4 color;

uniform float width;
uniform float height;
uniform float power;
uniform sampler2D MainTex;
uniform sampler2D MainTex2;
uniform sampler2D MainTex3;
uniform sampler2D depth;
uniform vec4 particleColor_s;
uniform vec4 particleColor_e;
uniform vec4 particleColor2_s;
uniform vec4 particleColor2_e;
uniform vec4 particleColor3_s;
uniform vec4 particleColor3_e;

float zNear = 0.1;
float zFar  = 200.0;

vec4 lerp(vec4 s, vec4 e, float l)
{
    return vec4(s.r + (e.r-s.r)*l, s.g + (e.g-s.g)*l, s.b + (e.b-s.b)*l, s.a + (e.a-s.a)*l);
}

float Contrast(float d)
{
	float val = clamp( 2.0*(d > 0.5? 1.0-d : d ), 0.0, 1.0);

	float a = 0.5 * pow(val, power);
	
	return (d > 0.5) ? 1.0 - a : a;

}

void main()
{
    vec4 albedo;
    vec4 color_s, color_e;
    vec2 screenScale = vec2(1.0/width, 1.0/height);
    if (tex_type < 1.1)
    {
        albedo = texture(MainTex, uv);
        color_s = particleColor_s;
        color_e = particleColor_e;
    }else if(tex_type < 2.1)
    {
        albedo = texture(MainTex2, uv);
        color_s = particleColor2_s;
        color_e = particleColor2_e;
    }
    else
    {
        float index = floor((1-life_lerp)/0.25);
        vec2 anim_uv = uv * 0.5;
        if (index == 0)
        {
            anim_uv += vec2(0.0, 0.0);
        }
        else if(index == 1)
        {
            anim_uv += vec2(0.0, 0.5);
        }
        else if(index == 2)
        {
            anim_uv += vec2(0.5, 0.5);
        }
        else
        {
            anim_uv += vec2(0.5, 0.0);
        }
        
        albedo = texture(MainTex3, anim_uv);
        color_s = particleColor3_s;
        color_e = particleColor3_e;
    }
    float screenDepth = texture(depth, projPos.xy/projPos.w).x;
    screenDepth = 2.0 * screenDepth - 1.0;
    screenDepth = 2.0 * zNear * zFar / (zFar + zNear - screenDepth * (zFar - zNear));
    float weight = screenDepth - d;
    // weight = clamp(weight * 1.0, 0.0, 1.0);
    weight = Contrast(weight);
    // color = vec4(vec3(weight), 1.0);
    vec4 none_blend_color = vec4(albedo.rgb, albedo.a * life_lerp) * lerp(color_s, color_e, 1-life_lerp);
    color = vec4(none_blend_color.rgb, none_blend_color.a * weight);
    //color = vec4(albedo.rgb, albedo.a *weight);
}