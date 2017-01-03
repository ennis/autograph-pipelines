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

function keyboardInput(key, state)
	ag.debug('keyboardInput: %i, %i', key, state)
	if key == 263 and state == 0 then
		cameraControl:rotate(-0.1, 0.0)
	elseif key == 262 and state == 0 then 
		cameraControl:rotate(0.1, 0.0)
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

-- called when the frame is being rendered
function render()
	--ag.debug('render (%ix%i), value=%f', framebufferWidth, framebufferHeight, value0)
	cam = cameraControl:getCamera()
	scene:update()
	sceneRenderer:renderScene(gBuffer, scene, cam)
	renderUtils:drawSprite(ag.getDefaultFramebuffer(), 
		0.0, 0.0, 1.0, 1.0, gBuffer.diffuseColor,
		0.0, 0.0, 1.0, 1.0)
	if showWireframe then
		wireframeRenderer:renderSceneObject(ag.getDefaultFramebuffer(), scene, obj, cam, wireframeOpaque)
	end
end
