#version 450
## include "utils.glsl"

## if _VERTEX_ then

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texcoord;
out vec2 fTexcoord;
void main() {
  gl_Position = vec4(pos, 0.0, 1.0);
  fTexcoord = texcoord;
}

## elseif _FRAGMENT_ then

layout(binding = 0) uniform sampler2D tex;
layout(location = 0) out vec4 color;
in vec2 fTexcoord;

void main() {
  color = texture(tex, fTexcoord.xy);
}

## end

