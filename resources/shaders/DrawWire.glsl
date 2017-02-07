#version 450
#include "MeshShader.glsli"

## if _VERTEX_ then

void main() {
  gl_Position = uViewProjMatrix * uModelMatrix * vec4(iPosition, 1.0f);
}

## elseif _FRAGMENT_ then

uniform vec4 uWireColor;

layout(location = 0) out vec4 color;

void main() {
  color = uWireColor;
}

## end

