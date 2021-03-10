#version 330 core
layout (triangles) in; //每个输入为一个三角形图元，即有3个顶点，即有3次顶点着色器执行
layout (triangle_strip, max_vertices = 3) out; // 输出类型为连接的三角形，最多输出3个顶点，即还是一个三角形

//输入的数据结构，与顶点着色器中的命名一致
in VS_OUT {
    vec2 texCoords;
    vec3 world_pos;
    vec3 world_norm;
} gs_in[];

//输出到片段着色器
out vec2 TexCoords; // 纹理坐标
out vec3 w_norm; // 世界空间法线
out vec3 w_pos; // 世界空间位置

uniform float scale; // 从cpp中输入的高度大小
uniform sampler2D height; // 高度图

//根据3个顶点计算其构成的三角形的法线
vec3 GetNormal(vec3 v1, vec3 v2, vec3 v3)
{
    //a，b为2个在平面上的不平行向量
    vec3 a = vec3(v1) - vec3(v2);
    vec3 b = vec3(v3) - vec3(v1);
    //叉乘得到法线
    return normalize(cross(a, b));
}

void main() {
    //三个输入顶点的纹理坐标采样高度图得到不同的灰度值
    float h1 = texture(height, gs_in[0].texCoords).r;
    float h2 = texture(height, gs_in[1].texCoords).r;
    float h3 = texture(height, gs_in[2].texCoords).r;

    //用该值在世界空间移动三个顶点的位置，即是y轴的偏移
    vec4 v1 = gl_in[0].gl_Position + vec4(gs_in[0].world_norm * h1 * scale, 0.0);
    vec4 v2 = gl_in[1].gl_Position + vec4(gs_in[1].world_norm * h2 * scale, 0.0);
    vec4 v3 = gl_in[2].gl_Position + vec4(gs_in[2].world_norm * h3 * scale, 0.0);

    // rlly fun effects
    // vec3 norm = GetNormal(v1.xyz, v2.xyz, v3.xyz);
    // v1 = gl_in[0].gl_Position + vec4(norm * h1 * scale, 0.0);
    // v2 = gl_in[1].gl_Position + vec4(norm * h2 * scale, 0.0);
    // v3 = gl_in[2].gl_Position + vec4(norm * h3 * scale, 0.0);

    // 每个EmitVertex()函数前是一次输出，即要输出上面的out标识的变量
    // 这里输出3次，是因为上面layout定义了输出的类型，即连接的三角形，即3个顶点
    gl_Position = v1;
    TexCoords = gs_in[0].texCoords;
    w_pos = gs_in[0].world_pos;
    w_norm = GetNormal(v1.xyz, v2.xyz, v3.xyz);// 移动后的三角形的法线
    EmitVertex();
    gl_Position = v2;
    TexCoords = gs_in[1].texCoords;
    w_pos = gs_in[1].world_pos;
    w_norm = GetNormal(v1.xyz, v2.xyz, v3.xyz);
    EmitVertex();
    gl_Position = v3;
    TexCoords = gs_in[2].texCoords;
    w_pos = gs_in[2].world_pos;
    w_norm = GetNormal(v1.xyz, v2.xyz, v3.xyz);
    EmitVertex();
    //三个顶点输出完成，即一个图元输出完成
    EndPrimitive();
}