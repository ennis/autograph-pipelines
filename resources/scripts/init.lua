fx = require 'autograph/fx'
core = require 'autograph/core'
gl = require 'autograph/gl'
glapi = require 'autograph/glapi'
ui = require 'autograph/ui'

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
		fillMode = glapi.GL_FILL,
		cullMode = glapi.GL_BACK,
		frontFace = glapi.GL_CCW
	},
	depthStencilState = {
		depthTestEnable = true,
		depthWriteEnable = true
	},
	blendState = {
		[0] = { 
			enabled = true,
			modeRGB = glapi.GL_FUNC_ADD,
			modeAlpha = glapi.GL_FUNC_ADD,
			funcSrcRGB = glapi.GL_SRC_ALPHA,
			funcDstRGB = glapi.GL_ONE_MINUS_SRC_ALPHA,
			funcSrcAlpha = glapi.GL_ONE,
			funcDstAlpha = glapi.GL_ZERO
		}
	},
	viewports = {
		[0] = { x = 0, y = 0, w = 1024, h = 1024 }
	},
	vertexShader = deferredVS,
	fragmentShader = deferredFS	
}

function loadMeshAction(path) end 

-- ui.dock {
-- 	-- if value != nil, then will create a variable holding the value
-- 	-- access the value with ui.var.curvatureScaling
-- 	ui.slider { label = 'Curvature Scaling', id = 'curvatureScaling', min = 0.01, max = 1000 }
-- 	ui.button { label = 'Load Mesh', action = 'loadMeshAction' }
-- 	ui.button { label = 'Save Mesh', action = 'saveMeshAction' }
-- 	ui.checkbox { label = 'Bypass stuff', id = 'bypassStuff' }
-- }

-- ui.menu {
-- 	label = '&File',
-- 	ui.menuItem { label = 'Close', action = 'closeAction' },
-- 	ui.menu { label = 'New' ,
-- 	}
-- }


--ui.addChild(ui.dock { ui.vbox { mainui.curvatureScaling, mainui.loadMesh, mainui.saveMesh }})

-- local loadFileDock = ui.dock {
-- 	ui.vbox {
-- 		label = 'File',
-- 		ui.button { label = 'Load Mesh', action = loadMeshAction },
-- 		ui.slider { label = 'Curvature scaling', target = 'curvatureScaling', min = 0.01, max = 100 }
-- 	}
-- }
-- ui.addChild(loadFileDock)


local scene = core.Scene()
local obj = scene:loadMesh(core.getActualPath('resources/mesh/youmu/youmu.fbx'))
local defaultPass = fx.createDrawPass(defaultEffect)

function init() 
	obj.transform:setPosition(0.0, 1.0, 0.0)
	core.debug('tr.position = %f,%f,%f', obj.transform.position.x, obj.transform.position.y, obj.transform.position.z)
end

local value0 = 0.0
local value1 = 0.0

function onRender()
	imgui.BeginMainMenuBar()
	if imgui.BeginMenu('File', true) then
		if imgui.MenuItem('New...', 'Ctrl+N', false, true) then
			core.debug('New!')
		end
		if imgui.MenuItem('Open...', 'Ctrl+O', false, true) then
			core.debug('Open')
		end
		imgui.EndMenu()
	end
	imgui.EndMainMenuBar()

	_, value0 = imgui.SliderFloat('value', value0, 0.0, 1.0, '%.3f', 1.0)

	core.debug('onRender (%ix%i), value=%f', framebufferWidth, framebufferHeight, value0)
	--sceneRenderer:render()
	--core.drawMesh(mesh, pipeline, { uniforms = {
	--	screen_size = types.vec2(screen_width, screen_height)
	--	} })
end
