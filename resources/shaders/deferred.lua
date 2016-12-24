gl = require 'glapi'

deferredShader =
{
	layout = {
		{ buffer = 0, type = gl.GL_FLOAT, size = 3, stride = 12, normalized = false },
		{ buffer = 0, type = gl.GL_FLOAT, size = 3, stride = 12, normalized = false },
		{ buffer = 0, type = gl.GL_FLOAT, size = 3, stride = 12, normalized = false },
		{ buffer = 0, type = gl.GL_FLOAT, size = 2, stride = 8, normalized = false }
	},
	rasterizerState = {
		fillMode = gl.GL_FILL,
		cullMode = gl.GL_BACK,
		frontFace = gl.GL_CCW
	},
	depthStencilState = {
		depthTestEnable = true,
		depthWriteEnable = true
	},
	blendState = {
		[0] = { 
			enabled = true,
			modeRGB = gl.GL_FUNC_ADD,
			modeAlpha = gl.GL_FUNC_ADD,
			funcSrcRGB = gl.GL_SRC_ALPHA,
			funcDstRGB = gl.GL_ONE_MINUS_SRC_ALPHA,
			funcSrcAlpha = gl.GL_ONE,
			funcDstAlpha = gl.GL_ZERO
		},
		[1] = { enabled = false }
	},
	shaderFile = 'deferred.glsl'
}

