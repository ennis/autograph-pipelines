#version 450

## if _VERTEX_ then

layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 color;

out vec4 fColor;

void main() {
  gl_Position = vec4(pos, 0.0, 1.0);
  fColor = color;
}

## elseif _FRAGMENT_ then

in vec4 fColor;
layout(location = 0) out vec4 color;

void main() {
  color = fColor;
}

## end

