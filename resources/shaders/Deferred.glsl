#version 450
#include "MeshShader.glsli"
#include "Utils.glsli"

## if _VERTEX_ then

out vec3 Nw0;
out vec3 Pv;
out vec3 Tv0;
out vec2 uv;

void main() {
  gl_Position = uViewProjMatrix * uModelMatrix * vec4(iPosition, 1.0f);
  // assume no scaling in modelMatrix
  Nw0 = (uModelMatrix * vec4(iNormal, 0.0)).xyz;
  Tv0 = (uViewMatrix * uModelMatrix * vec4(iTangent, 0.0)).xyz;
  Pv = (uViewMatrix * uModelMatrix * vec4(iPosition, 1.0)).xyz;
  uv = vec2(iTexcoords.x, 1-iTexcoords.y);
}

## elseif _FRAGMENT_ then

in vec3 Nw0;
in vec3 Pv;
in vec3 Tv0;
in vec2 uv;

layout(location = 0) out vec4 rtAlbedo;
layout(location = 1) out vec4 rtNormals;

layout(binding=0) uniform sampler2D uAlbedo;

void main() {
	vec3 Nw = normalize(Nw0);
  rtNormals = packNormal(Nw);
  rtAlbedo = texture(uAlbedo, uv); 
}

## end

