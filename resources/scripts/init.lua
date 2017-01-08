ag = require 'autograph'
gl = require 'glapi'

local scene = scene or ag.Scene()
local obj = obj or scene:loadModel('mesh/house_plant/house_plant')
local sceneRenderer = sceneRenderer or DeferredSceneRenderer()
local wireframeRenderer = wireframeRenderer or WireframeOverlayRenderer()
local gBuffer = gBuffer or DeferredGBuffer(1280, 720)
local showWireframe = showWireframe or false
local wireframeOpaque = wireframeOpaque or false
local cameraControl = cameraControl or ag.CameraControl()
local renderUtils = renderUtils or ag.RenderUtils()

local imgCaveMimi = ag.getTexture('img/cavestory/PrtMimi')
local imgCaveFall = ag.getTexture('img/cavestory/PrtFall')
local imgCaveSand = ag.getTexture('img/cavestory/PrtSand')

local texTest = ag.Texture.create2DMultisample(ag.ImageFormat.R8G8B8A8_UNORM, 512, 512, 8)

--local arcball = ag.Arcball()

-- called when the script is loaded or reloaded
function init() 
	reset()
end

-- called on reset
function reset()
	-- body
	obj.localTransform:setPosition(0.0, 0.0, 0.0)
	obj.localTransform:setScale(0.001)
	scene:update()
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
	cameraControl:setAspectRatio(screenWidth/screenHeight)
	cameraControl:setNearFarPlanes(0.01, 10.0)
	cameraControl:lookAt(cx, cy, cz)
	cameraControl:lookDistance(1.5*size)
	cameraControl:setFieldOfView(45.0)
end

-- called when the window has been resized
function resize(w, h)
	ag.debug('resize: %i,%i', w, h)
	cameraControl:setAspectRatio(w/h)
	gBuffer:release()
	gBuffer = DeferredGBuffer(w, h)
end

-- resource reload request
function reload()

end

local camSensitivity = 0.1

-- event
function onEvent(ev)
	if ev.type == autograph.EventType.Key then
		ag.debug('Event %i %i', ev.key, ev.action)
		if ev.key == ag.KEY_LEFT and ev.action == 0 then
			cameraControl:rotate(-0.1, 0.0)
		elseif ev.key == ag.KEY_RIGHT and ev.action == 0 then 
			cameraControl:rotate(0.1, 0.0)
		elseif ev.key == ag.KEY_W and ev.action == 0 then
			showWireframe = not showWireframe
		elseif ev.key == ag.KEY_O and ev.action == 0 then
			wireframeOpaque = not wireframeOpaque
		end
	end
end


-- called when the frame is being rendered
function render()
	--ag.debug('render (%ix%i), value=%f', framebufferWidth, framebufferHeight, value0)
	cam = cameraControl:getCamera()
	scene:update()
	sceneRenderer:renderScene(gBuffer, scene, cam)
	renderUtils:drawSprite(ag.getDefaultFramebuffer(), 
		0, 0, 640, 480, gBuffer.diffuseColor,
		0.0, 0.0, 1.0, 1.0)
	if showWireframe then
		wireframeRenderer:renderSceneObject(ag.getDefaultFramebuffer(), scene, obj, cam, wireframeOpaque)
	end
end
