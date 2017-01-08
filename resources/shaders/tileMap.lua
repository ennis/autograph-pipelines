gl = require 'glapi'

local meshLayout = {
		{ buffer = 0, type = gl.GL_FLOAT, size = 2, stride = 8, normalized = false },
		{ buffer = 0, type = gl.GL_FLOAT, size = 2, stride = 8, normalized = false }
	}

local defaultBlendState = {
	[0] = { 
			enabled = true,
			modeRGB = gl.GL_FUNC_ADD,
			modeAlpha = gl.GL_FUNC_ADD,
			funcSrcRGB = gl.GL_SRC_ALPHA,
			funcDstRGB = gl.GL_ONE_MINUS_SRC_ALPHA,
			funcSrcAlpha = gl.GL_ONE,
			funcDstAlpha = gl.GL_ZERO
		}
}

tileMap =
{
	layout = meshLayout,
	rasterizerState = {
		fillMode = gl.GL_FILL,
		cullMode = gl.GL_BACK,
		frontFace = gl.GL_CCW
	},
	depthStencilState = {
		depthTestEnable = false,
		depthWriteEnable = false
	},
	blendState = {
		[0] = defaultBlendState
	},
	shaderFile = 'tileMap.glsl'
}
