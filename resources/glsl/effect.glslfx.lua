require 'fxcommon'

-- sampler definitions
local samplerNearest = {
    minFilter = Nearest,
    magFilter = Nearest,
    addrU = Clamp,
    addrV = Clamp
}

local noBlendState = { enabled = false }

local shadowMap = RenderTexture
{
    format = TextureFormat.RGBA8,
    width = 2048,
    height = 2048,
    type = TextureType.Texture2D
}

-- shared code
shared = [[
float PI = 3.14159265359;
float TWOPI = 2.0*3.14159265359;
// Generalized blending ('over' operator)
vec4 blend(in vec4 S, in vec4 D)
{
	vec4 O;
	O.rgb = S.rgb * S.a + D.rgb * D.a * (1.0 - S.a);
	O.a = S.a + D.a * (1.0 - S.a);
	O.rgb /= O.a;
	return O;
}

float sq(float x)
{
	return x*x;
}
]]

-- blur
local blur = [[
#version 450
#include "utils.glsl"

#if FMT_RGBA8
#define FORMAT rgba8
#elif defined FMT_RGBA32F
#define FORMAT rgba32f
#elif defined FMT_R32F
#define FORMAT r32f
#endif

layout(binding = 0, FORMAT) readonly uniform image2D tex0;
layout(binding = 1, FORMAT) writeonly uniform image2D tex1;
layout(binding = 0, std140) uniform U0 {
  int blurSize;
  float sigma;
  float sstep_low;
  float sstep_high;
};

layout(local_size_x = 16, local_size_y = 16) in;

vec4 linstep(float edge0, float edge1, vec4 x)
{
  vec4 t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
  return mix(vec4(0.0f), vec4(1.0f), t);
}

void main() {
  ivec2 texelCoords = ivec2(gl_GlobalInvocationID.xy);
  float numBlurPixelsPerSide = float(blurSize / 2);

#ifdef BLUR_H
  ivec2 blurMultiplyVec = ivec2(1, 0);
#else
  ivec2 blurMultiplyVec = ivec2(0, 1);
#endif

  // Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)
  vec3 incrementalGaussian;
  incrementalGaussian.x = 1.0 / (sqrt(TWOPI) * sigma);
  incrementalGaussian.y = exp(-0.5 / (sigma * sigma));
  incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;

  vec4 avgValue = vec4(0.0, 0.0, 0.0, 0.0);
  float coefficientSum = 0.0;

  // Take the central sample first...
  avgValue += imageLoad(tex0, texelCoords) * incrementalGaussian.x;
  coefficientSum += incrementalGaussian.x;
  incrementalGaussian.xy *= incrementalGaussian.yz;

  // Go through the remaining 8 vertical samples (4 on each side of the center)
  for (int i = 1; i <= numBlurPixelsPerSide; i++) {
    avgValue += imageLoad(tex0, texelCoords - i * blurMultiplyVec) *
                incrementalGaussian.x;
    avgValue += imageLoad(tex0, texelCoords + i * blurMultiplyVec) *
                incrementalGaussian.x;
    coefficientSum += 2.0 * incrementalGaussian.x;
    incrementalGaussian.xy *= incrementalGaussian.yz;
  }

  imageStore(tex1, texelCoords, linstep(sstep_low, sstep_high, avgValue / coefficientSum));
  //imageStore(tex1, texelCoords, imageLoad(tex0, texelCoords));
  memoryBarrierImage();
}
]]

-- resources are allocated only if they are used in a technique somewhere
gbuffer = { }
gbuffer.normals = { }
gbuffer.depth = { }
gbufferFBO = { color = { gbuffer.albedo, gbuffer.normals }, depth = gbuffer.depth }

-- configuration parameters
RT_width = ConfigInt
RT_height = ConfigInt
-- dynamic uniforms shared between programs
blurSize = UniformFloat

local mainTechnique = {
    -- passes are applied in the defined order
    passes =
    {
        render =
        {
            vertexShader = GBufferVS,
            fragmentShader = GBufferFS,
            blendState = noBlend,
            depthStencilState = depthTest_StencilWrite,
            target = { color = { gbuffer.albedo, gbuffer.normals }, depth = gbuffer.depth }
        },
        blurH =
        {
            computeShader = blur,
            computeShaderDefines = { "BLUR_H" },
            images = { gbuffer.normals, tmp.blur }
        },
        blurV =
        {

        }
    },
} 
