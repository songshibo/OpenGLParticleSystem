#version 330 core
layout (location = 0) in vec3 aPos; //模型的顶点数据，顶点着色器每个顶点执行一次
layout (location = 1) in vec3 aNormal; // 模型的法线数据
layout (location = 2) in vec2 aTexCoords; // 模型的纹理坐标

//顶点着色器的输出结构体，到几何着色器中
out VS_OUT {
    vec2 texCoords;//输出纹理坐标
    vec3 world_pos;//世界空间的坐标位置
    vec3 world_norm;//世界空间的法线向量
} vs_out;

uniform mat4 projection; //相机的投影矩阵
uniform mat4 view; // 相机的观察矩阵
uniform mat4 model; //plane这个模型的模型矩阵

void main()
{
    //aNormal原始数据为模型空间的法线，model矩阵将模型转换到世界空间
    vs_out.world_norm = normalize(mat3(model) * aNormal);
    vs_out.world_pos = vec3(model * vec4(aPos, 1.0));
    //纹理坐标不变
    vs_out.texCoords = aTexCoords;
    //裁剪空间坐标为必须计算
    gl_Position = projection * view * model * vec4(aPos, 1.0); 
}