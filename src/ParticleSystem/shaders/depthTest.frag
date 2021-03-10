#version 330 core

out vec4 FragColor;

in vec4 projPos;
in float d;

uniform sampler2D depthTex;

float zNear = 0.1;
float zFar  = 200.0;

float LinearEyeDepth(float z)
{
    vec4 _ZBufferParams;
    _ZBufferParams.x = 1 - zFar/zNear;
    _ZBufferParams.y = zFar/zNear;
    _ZBufferParams.z = _ZBufferParams.x / zFar;
    _ZBufferParams.w = _ZBufferParams.y / zFar;

    return 1.0 / (_ZBufferParams.z * z + _ZBufferParams.w);
}

void main()
{
    float rawZ = texture(depthTex, projPos.xy/projPos.w).x;
    // screenDepth = 2.0 * screenDepth - 1.0;
    // screenDepth = 2.0 * zNear * zFar / (zFar + zNear - screenDepth * (zFar - zNear));
    float sceneZ = LinearEyeDepth(rawZ);
    float diff = sceneZ - d;
    if (rawZ > 0.0)
    {
         FragColor = vec4(diff, diff, diff, 1.0);
    }else
    {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}