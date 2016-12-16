ag = require 'autograph'
gl = require 'glapi'

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

local scene = ag.Scene()
local camera = ag.Camera()
local obj = scene:loadMesh(ag.getActualPath('resources/mesh/youmu/youmu.fbx'))
local defaultPass = ag.createDrawPass(defaultEffect)
local sceneRenderer = ag.DeferredSceneRenderer()
local gBuffer = ag.DeferredGBuffer(1280, 720)

function init() 
	obj.transform:setPosition(0.0, 1.0, 0.0)
	ag.debug('tr.position = %f,%f,%f', obj.transform.position.x, obj.transform.position.y, obj.transform.position.z)
end

local value0 = 1.0

function resize(w, h)
	ag.debug('resize: %i,%i', w, h)
end

function render()
	--ag.debug('render (%ix%i), value=%f', framebufferWidth, framebufferHeight, value0)
	sceneRenderer:renderScene(gBuffer, scene, camera)
end
