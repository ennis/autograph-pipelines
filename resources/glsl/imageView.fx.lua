require 'fxcommon'

-- Check for valid config options
Config.int 'width'
Config.int 'height'
Config.int 'smWidth'
Config.int 'smHeight'

-- Declare uniforms
Uniform.int 'u_zoom'
Uniform.ivec2 'u_size'
Uniform.texture 'u_tex0'
Uniform.target 'rt_target'
Uniform.buffer 'u_sceneData'
Uniform.ssbo 'u_particles'

-- Declare resources
Texture2D.new { 
	name = 'shadowMap',
	dim = Texture.Texture2D 
	format = Texture.RGBA8, 
	width = 2048, 
	height = 2048 }

Sampler.new {
	name = 'samLinear',
	addrU = Sampler.Clamp,
	addrV = Sampler.Clamp,
	minFilter = Sampler.Linear,
	magFilter = Sampler.Linear
}

-- Declare passes
Pass.new {
	name = 'drawBackground',
	vertexShader = { VS },
	fragmentShader = { BackgroundFS },
	target = { color = rt_target, depth = nil }
	uniforms = { size = u_size }
}

Pass.new {
	name = 'renderShadowMap',
	vertexShader = { ShadowVS },
	fragmentShader = { ShadowFS }
	target = { color = rt_ }
}

Texture2D('rendertarget', {
	dim = 
});



-- Shaders
VS = [[
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texcoord;
out vec2 fTexcoord;
void main() {
  gl_Position = vec4(pos, 0.0, 1.0);
  fTexcoord = texcoord;
}
]]

BackgroundFS = [[
fragment shader that draws the background grid
]]

FS = [[
layout(binding=0) uniform sampler2D tex0;
// fragment shader that samples the texture 
]]

BlendModeMultiply = [[
vec4 screen(in vec4 S, in vec4 D)
{
	vec4 O;
	O.rgb = vec3(1.0) - (vec3(1.0) - S.rgb*S.a) * (vec3(1.0) - D.rgb*D.a);
	O = apply_alpha(S, D, O);
	return clamp(O, 0.0f, 1.0f);
}
]]

-- Resources
if config.blendMode = "multiply"
	blendFS = { blendCommonFS, BlendModeMultiplyFS }
else if config.blendMode = "screen"
end 

-- Techniques
imageView = Technique {
	drawBackground = Pass {
		vertexShader = { VS },
		fragmentShader = { BackgroundFS },
		target = { color: target, depth: nil }
	},
	drawTexture = Pass {
		vertexShader = { VS },
		fragmentShader = { FS },
		target = { color: target, depth: nil },
		uniforms = {
			zoom = u_zoom
		},
		textures = {
			tex0 = { u_tex0, samLinear }
		}
	}
}

paintBrush = PaintTechnique { 

}

opaque = GeometryPass {
	dependsOn { final },
}

toneMap = ScreenPass {
	dependsOn = { ... }
}

final = ScreenPass {
	dependsOn = { ... }
	fragmentShader = ...

}

GeometryPass('opaque', {
	dependsOn = nil,
	vertexShader = ...,
})