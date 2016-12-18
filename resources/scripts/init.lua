ag = require 'autograph'
gl = require 'glapi'

local scene = ag.Scene()
local camera = ag.Camera()
local obj = scene:loadMesh(ag.getActualPath('resources/mesh/youmu/youmu.fbx'))
--local defaultPass = ag.createDrawPass(defaultEffect)
local sceneRenderer = ag.DeferredSceneRenderer(g_shaderManager)
local gBuffer = ag.DeferredGBuffer(1280, 720)

function init() 
	obj.transform:setPosition(0.0, 1.0, 0.0)
	ag.debug('tr.position = %f,%f,%f', obj.transform.position.x, obj.transform.position.y, obj.transform.position.z)
end

function resize(w, h)
	ag.debug('resize: %i,%i', w, h)
end

function render()
	--ag.debug('render (%ix%i), value=%f', framebufferWidth, framebufferHeight, value0)
	sceneRenderer:renderScene(gBuffer, scene, camera)
end
