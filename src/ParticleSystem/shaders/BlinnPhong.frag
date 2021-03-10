#version 330 core
out vec4 frag_color;

in vec3 world_pos;
in vec3 world_norm;
in vec2 uv;

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform vec3 base_color;
uniform vec3 viewPos;
uniform Light light;
uniform Material material;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 衰减
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));    
    // 合并结果
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, uv));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, uv));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    return (ambient + diffuse);
}

void main()
{
    // ambient
    vec3 ambient = light.ambient;//* vec3(texture(material.diffuse, uv));
  	
    // diffuse 
    vec3 norm = normalize(world_norm);
    vec3 lightDir = normalize(-light.direction);  
    float diff = dot(norm, lightDir)*0.5 + 0.5;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, uv).rgb * base_color;
    
    // specular
    vec3 viewDir = normalize(viewPos - world_pos);
    vec3 halfDir = normalize(viewDir + lightDir);  
    float spec = pow(max(dot(viewDir, halfDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, uv).r;  
        
    vec3 result = ambient + diffuse + specular;

    vec3 point = CalcPointLight(pointLights[0], norm, world_pos, viewDir);

    frag_color = vec4(result + point, 1.0);
}