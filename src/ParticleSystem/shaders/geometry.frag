#version 330 core
out vec4 FragColor; // 输出像素颜色

//平行光
struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
};

//材质，仅包含漫反射贴图
struct Material {
    sampler2D diffuse;
}; 

//相机位置
uniform vec3 viewPos;
uniform Light light;
uniform Material material;

//与几何着色器的输出对应
in vec2 TexCoords;
in vec3 w_norm;
in vec3 w_pos;

uniform sampler2D diffuse;

void main()
{
    // ambient
    vec3 ambient = light.ambient;
  	
    // diffuse 
    //在世界空间下计算光照
    vec3 norm = normalize(w_norm);
    vec3 lightDir = normalize(-light.direction);  
    float diff = dot(norm, lightDir)*0.5 + 0.5; // Half Lambert 光照模型，相对Lambert更亮一些
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;// 漫反射颜色
        
    vec3 result = ambient + diffuse; //最终输出为漫反射和环境光的和
    FragColor = vec4(result, 1.0);// alpha是1.0即不透明
}
