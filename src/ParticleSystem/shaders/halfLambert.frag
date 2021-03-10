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

uniform vec3 base_color;
uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main()
{
    // ambient
    vec3 ambient = light.ambient;
  	
    // diffuse 
    vec3 norm = normalize(world_norm);
    vec3 lightDir = normalize(-light.direction);  
    float diff = dot(norm, lightDir)*0.5 + 0.5;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, uv).rgb * base_color;
    
    // specular
    vec3 viewDir = normalize(viewPos - world_pos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec;  
        
    vec3 result = ambient + diffuse; //+ specular;
    frag_color = vec4(result, 1.0);
}