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
		}
	},
	shaderFile = 'deferred.glsl'
}


-- One file = one pass => NO
-- One file = multiple passes
-- One file = template for passes
-- 		1. Load shader file in context
--		2. call createShaderPass(shaderId, params)
--			2.1. call a function defined in the shader file that returns a complete state
--		3. get shader!
-- From C++:
-- 		shaderMan.load("resources/shaders/deferred")
--		shaderMan.getPass("deferred")
-- Multi-compile:
-- 		shaderMan.load("resources/shaders/blurCompute")
--		blurHPass = shaderMan.getPass("blurCompute", "BLUR_H")
--		blurVPass = shaderMan.getPass("blurCompute", "BLUR_V")
-- From Lua:
-- require 'shaders/deferred'
-- blurH = loadShaderPass('blurCompute', 'BLUR_H')

-- one source => multiple shader passes (one for each combination of keywords)
-- one source => infinite number of shader passes (float constant parameters) 
-- the question boils down to: do we want to be able to run arbitrary code in shader templates
--	VS only preprocessor keywords and simple text replacement