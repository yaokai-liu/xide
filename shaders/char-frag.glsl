#version 460 core
layout (location = 4) uniform sampler2D tex;
in  vec4 vsColor;
in  vec2 texCoord;
out vec4 color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, texCoord.xy).r);
    color = vsColor * sampled;
}
