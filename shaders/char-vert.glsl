#version 460 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aCol;
layout (location = 2) in vec2 aTex;
layout (location = 3) uniform vec4 viewport;
out vec4 vsColor;
out vec2 texCoord;

void main()
{
    vec2 ndcPosition = (aPos.xy / viewport.zw) * 2.0f - 1.0f;
    ndcPosition.y = -ndcPosition.y;
    gl_Position = vec4(ndcPosition, aPos.z, 1.0f);
    vsColor = aCol;
    texCoord = aTex;
}