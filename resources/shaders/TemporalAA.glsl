#version 450
#include "Utils.glsli"

layout(binding=0) uniform sampler2D uPrev; 
layout(binding=1) uniform sampler2D uFrame; 
layout(binding=2) uniform sampler2D uVelocity; 
layout(binding=0, rgba16f) writeonly uniform image2D uNext; 

layout(local_size_x = 32, local_size_y = 32) in;

layout(std140, binding = 0) uniform CameraParameters {
  mat4 uViewMatrix;
  mat4 uProjMatrix;
  mat4 uViewProjMatrix;
  mat4 uInvProjMatrix;
  mat4 uPrevViewProjMatrixVelocity;
  mat4 uViewProjMatrixVelocity;
};

uniform float uWeight;
uniform vec2 uSampleOffset;

void main() {
  ivec2 P = ivec2(gl_GlobalInvocationID.xy);
  vec2 size = textureSize(uPrev, 0);
  vec2 uv = vec2(P+0.5)/size;

  vec4 Ca = texture(uFrame, uv);
  // reproject previous frame
  vec4 velocity = texture(uVelocity, uv);
  vec2 reprojPos = uv - 0.5*velocity.xy; 
  vec4 A = texture(uPrev, reprojPos);
  // exponential moving average
  vec4 B = uWeight*Ca + (1-uWeight)*A; 
   imageStore(uNext, P, B);
 // imageStore(uNext, P, vec4(reprojPos,0,1));
  //memoryBarrierImage();
}

