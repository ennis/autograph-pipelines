require 'gl'

local CS = [[
#version 450
#include "utils.glsl"
// Adapted from:
// http://callumhay.blogspot.com/2010/09/gaussian-blur-shader-glsl.html

layout(binding = 0, FORMAT) readonly uniform image2D tex0;
layout(binding = 1, FORMAT) writeonly uniform image2D tex1;
layout(binding = 0, std140) uniform U0 {
  int blurSize;
  float sigma;
  float sstep_low;
  float sstep_high;
};

layout(local_size_x = LOCAL_SIZE_X, local_size_y = LOCAL_SIZE_Y) in;

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


Shader('blurComputeH', {
	computeShaderFile = 'blurCompute.glsl',
	defines = { BLUR_H = true }	
})

Shader('blurComputeV', {
	computeShaderFile = 'blurCompute.glsl',
	defines = { BLUR_V = true }
})


-- One file = one pass => NO
-- One file = multiple passes
-- One file = template for passes
-- 		1. Load shader file in context
--		2. call createShaderPass(shaderId, params)
--			2.1. call a function defined in the shader file that returns a complete state
--		3. get shader!
-- From C++:
-- 		shaderMan.load("resources/shaders/deferred")
--			actually just does L.require("resources/shaders/deferred")
--		shaderMan.getPass("deferred")
--			calls autograph.createShaderPass('deferred', {}) and returns a table, then calls DrawPassBuilder::loadFromTable()
-- Multi-compile:
-- 		shaderMan.load("resources/shaders/blurCompute")
--		blurHPass = shaderMan.getPass("blurComputeH", "FORMAT=rgba8")
--		blurVPass = shaderMan.getPass("blurComputeV", "FORMAT=rgba8")
-- From Lua:
-- require 'shaders/blurCompute'
-- blurH = loadShaderPass('blurCompute', 'BLUR_H')

-- one source => multiple shader passes (one for each combination of keywords)
-- one source => infinite number of shader passes (float constant parameters) 
-- the question boils down to: do we want to be able to run arbitrary code in shader templates
--	VS only preprocessor keywords and simple text replacement => this one
-- Lua: createShaderPass(shader, params)