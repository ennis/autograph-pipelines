#version 450

## if _VERTEX_ then
layout(std140, binding = 0) uniform U0 {
  mat4 modelMatrix;
  mat4 viewMatrix;
  mat4 projMatrix;
  mat4 viewProjMatrix;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 texcoords;

void main() {
  gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
}

## elseif _FRAGMENT_ then

layout(location = 0) out vec4 color;

void main() {
  color = vec4(1.0, 0.0, 0.0, 1.0);
}

## end

