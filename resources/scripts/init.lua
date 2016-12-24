ag = require 'autograph'
gl = require 'glapi'

local scene = ag.Scene()
local obj = scene:loadModel(ag.getActualPath('resources/mesh/youmu/youmu.fbx'))
--local defaultPass = ag.createDrawPass(defaultEffect)
local sceneRenderer = ag.DeferredSceneRenderer(g_shaderManager)
local wireframeRenderer = ag.WireframeOverlayRenderer(g_shaderManager)
local gBuffer = ag.DeferredGBuffer(1280, 720)
local showWireframe = false
local wireframeOpaque = false

function init() 
	obj.localTransform:setPosition(0.0, 0.0, 0.0)
	--ag.debug("aabb: <%f,%f,%f>,<%f,%f,%f>", aabb.xmin, aabb.ymin, aabb.zmin, aabb.xmax, aabb.ymax, aabb.zmax)
	-- object autoscale
	-- camera center
	local aabb = obj:getApproximateWorldBoundingBox()
	ag.debug("aabb: <%f,%f,%f>,<%f,%f,%f>", aabb.xmin, aabb.ymin, aabb.zmin, aabb.xmax, aabb.ymax, aabb.zmax)
	local size = math.max(
		math.abs(aabb.xmax - aabb.xmin), 
		math.abs(aabb.ymax - aabb.ymin), 
		math.abs(aabb.zmax - aabb.zmin))
	local cx = (aabb.xmax + aabb.xmin) / 2
	local cy = (aabb.ymax + aabb.ymin) / 2
	local cz = (aabb.zmax + aabb.zmin) / 2
	--ag.debug("lookAt: <%f,%f,%f>", cx, cy, cz)
	g_mainCameraControl:setAspectRatio(g_framebufferWidth/g_framebufferHeight)
	g_mainCameraControl:setNearFarPlanes(0.01, 2*size)
	g_mainCameraControl:lookAt(cx, cy, cz)
	g_mainCameraControl:lookDistance(size)
	g_mainCameraControl:setFieldOfView(45.0)
end

function resize(w, h)
	ag.debug('resize: %i,%i', w, h)
	g_mainCameraControl:setAspectRatio(w/h)
end

local camSensitivity = 0.1

function keyboardInput(key, state)
	ag.debug('keyboardInput: %i, %i', key, state)
	if key == 263 and state == 0 then
		g_mainCameraControl:rotate(-0.1, 0.0)
	elseif key == 262 and state == 0 then 
		g_mainCameraControl:rotate(0.1, 0.0)
	elseif key == 90 and state == 0 then
		showWireframe = not showWireframe
	elseif key == 79 and state == 0 then
		wireframeOpaque = not wireframeOpaque
	end
end

function characterInput(char)
	ag.debug('characterInput: %c', char)
end

function mouseInput(x, y)
	ag.debug('mouseInput: %f, %f', x, y)
end

function mouseButton(button, x, y) 
	ag.debug('mouseButton: %i, %f, %f', button, x, y)
end

function render()
	--ag.debug('render (%ix%i), value=%f', framebufferWidth, framebufferHeight, value0)
	cam = g_mainCameraControl:getCamera()
	sceneRenderer:renderScene(gBuffer, scene, cam)
	if showWireframe then
		wireframeRenderer:renderSceneObject(ag.getDefaultFramebuffer(), scene, obj, cam, wireframeOpaque)
	end
end
