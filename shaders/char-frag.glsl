#version 460 core
layout (location = 4) uniform sampler2D tex;
in  vec4 vsColor;
in  vec2 TexCoords;
out vec4 color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, TexCoords).r);
    color = vsColor * sampled;
}
