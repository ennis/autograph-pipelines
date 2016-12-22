ag = require 'autograph'
gl = require 'glapi'

local scene = ag.Scene()
local obj = scene:loadMesh(ag.getActualPath('resources/mesh/youmu/youmu.fbx'))
--local defaultPass = ag.createDrawPass(defaultEffect)
local sceneRenderer = ag.DeferredSceneRenderer(g_shaderManager)
local wireframeRenderer = ag.WireframeOverlayRenderer(g_shaderManager)
local gBuffer = ag.DeferredGBuffer(1280, 720)

function init() 
	obj.transform:setPosition(0.0, 0.0, 0.0)
	local aabb = obj:getLocalBoundingBox()
	--ag.debug("aabb: <%f,%f,%f>,<%f,%f,%f>", aabb.xmin, aabb.ymin, aabb.zmin, aabb.xmax, aabb.ymax, aabb.zmax)
	-- object autoscale
	local size = math.max(
		math.abs(aabb.xmax - aabb.xmin), 
		math.abs(aabb.ymax - aabb.ymin), 
		math.abs(aabb.zmax - aabb.zmin))	
	obj.transform:setScale(1.0/size)
	-- camera center
	local waabb = obj:getApproximateWorldBoundingBox()
	local cx = (waabb.xmax + waabb.xmin) / 2
	local cy = (waabb.ymax + waabb.ymin) / 2
	local cz = (waabb.zmax + waabb.zmin) / 2
	--ag.debug("lookAt: <%f,%f,%f>", cx, cy, cz)
	g_mainCameraControl:lookAt(cx, cy, cz)
	g_mainCameraControl:lookDistance(1.0)
	g_mainCameraControl:setFieldOfView(45.0)
end

function resize(w, h)
	ag.debug('resize: %i,%i', w, h)
	g_mainCameraControl:setAspectRatio(w/h)
end

local camSensitivity = 0.1

function input(ev)
end

function render()
	--ag.debug('render (%ix%i), value=%f', framebufferWidth, framebufferHeight, value0)
	cam = g_mainCameraControl:getCamera()
	sceneRenderer:renderScene(gBuffer, scene, cam)
	wireframeRenderer:renderSceneObject(ag.getDefaultFramebuffer(), scene, obj, cam)
end
