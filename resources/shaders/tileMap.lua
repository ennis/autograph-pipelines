require 'shaders/utils'

tileMap = Geometry2DPass
{
	depthStencilState = {
		depthTestEnable = false,
		depthWriteEnable = false
	},
	shaderFile = 'tileMap.glsl'
}
