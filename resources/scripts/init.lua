core = require 'core'
gl = require 'gl'

local ImageFormat = {
  R32G32B32A32_SFLOAT = 0,
  R16G16B16A16_SFLOAT = 1,
  R32G32_SFLOAT = 2,
  R32_SFLOAT = 3,
  R8_UNORM = 4,
  R8G8B8A8_UNORM = 5,
  R8G8B8A8_SNORM = 6,
  B10G11R11_UFLOAT_PACK32 = 7,
  D32_SFLOAT = 8
}

function loadMesh(id)
	local fullpath = core.getActualPath(id)
	local mesh = core.Mesh.loadFromFile(fullpath)
	print(string.format('loading mesh %s (full path %s)', id, fullpath))
	return mesh
end 

local deferredVS = [[
#version 450
layout(std140, binding = 0) uniform U0 {
  mat4 modelMatrix;
  mat4 viewMatrix;
  mat4 projMatrix;
  mat4 viewProjMatrix;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 texcoords;
out vec3 Nw0;
out vec3 Pv;
out vec3 Tv0;
out vec2 fTexcoords;
void main() {
  gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
  // assume no scaling in modelMatrix
  Nw0 = (modelMatrix * vec4(normal, 0.0)).xyz;
  Tv0 = (viewMatrix * modelMatrix * vec4(tangent, 0.0)).xyz;
  Pv = (viewMatrix * modelMatrix * vec4(position, 1.0)).xyz;
  fTexcoords = texcoords;
}
]]

local deferredFS = [[
#version 450
in vec3 Nw0;
in vec3 Pv;
in vec3 Tv0;
in vec2 fTexcoords;

layout(location = 0) out vec4 rtStencil;
layout(location = 1) out vec4 rtPositions;
layout(location = 2) out vec4 rtNormals;
layout(location = 3) out vec4 rtDepth;
layout(location = 4) out vec4 rtTangents;

void main() {
  rtPositions = vec4(Pv, 1.0f);
  rtNormals = vec4(Nw0 / 2.0 + vec3(0.5), 1.0f);
  rtStencil = vec4(1.0);
} 
]]


local defaultEffect = {
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
	viewports = {
		[0] = { x = 0, y = 0, w = 1024, h = 1024 }
	},
	vertexShader = deferredVS,
	fragmentShader = deferredFS	
}

local function createDrawPass(params)
	local builder = core.DrawPassBuilder()
	if params.rasterizerState then 
		builder:setRasterizerState(params.rasterizerState)
	end
	if params.depthStencilState then
		builder:setDepthStencilState(params.depthStencilState)
	end
	if params.blendState then 
		for key, bs in pairs(params.blendState) do
			builder:setBlendState(key, bs)
	    end
	end
	if params.viewports then 
		for key, vp in pairs(params.viewports) do
			builder:setViewport(key, vp.x, vp.y, vp.w, vp.h)
		end
	end
	builder:setVertexShader(params.vertexShader)
	builder:setFragmentShader(params.fragmentShader)
	return builder:makeDrawPass()
end

local scene = core.Scene()
local obj = scene:loadMesh(core.getActualPath('resources/meshes/hogarth.obj'))
local defaultPass = createDrawPass(defaultEffect)


function init() 
	obj.transform:setPosition(0.0, 1.0, 0.0)
	print(string.format('tr.position = %f,%f,%f', obj.transform.position.x, obj.transform.position.y, obj.transform.position.z))
end

function onRender()
	print(string.format('onRender (%ix%i)', screen_width, screen_height))
	-- sceneRenderer.render()

	--core.drawMesh(mesh, pipeline, { uniforms = {
	--	screen_size = types.vec2(screen_width, screen_height)
	--	} })
end

-- issue: holding a reference to a mesh that is created in lua and referenced in a scene
-- once the reference is given to the scene (c++ side), the object on the Lua side can be garbage-collected, but it shouldn't
-- => use shared_ptr? 

-- hot-reloading of scripts?
-- invalidates all refs to meshes, textures, etc.