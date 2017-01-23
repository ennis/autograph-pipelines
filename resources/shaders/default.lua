require 'shaders/utils'

deferred = GeometryPass
{
	shaderFile = 'deferred.glsl'
}

drawSprite = Geometry2DPass
{
	depthStencilState = {
		depthTestEnable = false,
		depthWriteEnable = false
	},
	shaderFile = 'drawSprite.glsl'
}

wireframeOverlay = GeometryPass
{
	depthStencilState = {
		depthTestEnable = true,
		depthWriteEnable = false
	},
	shaderFile = 'wireframe.glsl'
}

wireframeOverlayNoDepth = GeometryPass
{
	depthStencilState = {
		depthTestEnable = false,
		depthWriteEnable = false
	},
	shaderFile = 'wireframe.glsl'
}

----------------------------------------------------------
-- Blur passes
local function BlurPasses(defs)
	local H = ComputeShader {
		defines = deepcopy(defs),
		shaderFile = 'blur.glsl',
		barrierBits = bit.bor(gl.GL_TEXTURE_FETCH_BARRIER_BIT, gl.GL_SHADER_IMAGE_ACCESS_BARRIER_BIT)
	}
	local V = ComputeShader {
		defines = deepcopy(defs),
		shaderFile = 'blur.glsl',
		barrierBits = bit.bor(gl.GL_TEXTURE_FETCH_BARRIER_BIT, gl.GL_SHADER_IMAGE_ACCESS_BARRIER_BIT)
	}
	H.defines.BLUR_H = 1
	V.defines.BLUR_V = 1
	return H,V
end

blurH_RGBA16F, blurV_RGBA16F = BlurPasses
{
	IN_FORMAT = rgba16f,
	OUT_FORMAT = rgba16f,
	ALPHA_PREMULT = 0
}

blurH_RGBA16F_AlphaPremult, blurV_RGBA16F_AlphaPremult = BlurPasses
{
	IN_FORMAT = rgba16f,
	OUT_FORMAT = rgba16f,
	ALPHA_PREMULT = 1
}


blurH_RGBA8, blurV_RGBA8 = BlurPasses
{
	IN_FORMAT = rgba16f,
	OUT_FORMAT = rgba16f,
	ALPHA_PREMULT = 0
}

blurH_RGBA8_AlphaPremult, blurV_RGBA8_AlphaPremult = BlurPasses
{
	IN_FORMAT = rgba8,
	OUT_FORMAT = rgba8,
	ALPHA_PREMULT = 1
}

blurH_R32F, blurV_R32F = BlurPasses
{
	IN_FORMAT = r32f,
	OUT_FORMAT = r32f,
	ALPHA_PREMULT = 1
}
