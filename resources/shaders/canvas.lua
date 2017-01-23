require 'shaders/utils'

gbuffer = GeometryPass{
	shaderFile = 'canvas_gbuffers.glsl'
}

evaluation = ScreenPass{
	shaderFile = 'canvas_evaluate.glsl'
}