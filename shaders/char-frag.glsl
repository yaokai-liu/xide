#version 460 core
layout (location = 5) uniform sampler2D tex;
in  vec4 vsColor;
in  vec2 texCoord;
out vec4 color;

void main()
{
  float hint = texture(tex, texCoord.xy).r;
  vec4 sampled = vec4(1.0, 1.0, 1.0, hint);
  color = vsColor * sampled;
}
