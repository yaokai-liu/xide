#version 460 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aCol;
layout (location = 3) uniform vec2 windowSize;
out vec4 vsColor;

void main()
{
    vec2 ndcPosition = (aPos.xy / windowSize) * 2.0f - 1.0f;
    ndcPosition.y = -ndcPosition.y;
    gl_Position = vec4(ndcPosition, aPos.z, 1.0f);
    vsColor = aCol;
}
