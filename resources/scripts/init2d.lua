ag = require 'autograph'
gl = require 'glapi'
require 'class'

local renderUtils = renderUtils or ag.RenderUtils()

local viewX = 0
local viewY = 0
local viewWidth = 640
local viewHeight = 480

local moveSpeed = 500
local shakeIntensity = 4.0

-- called when the script is loaded or reloaded
function init() 
	reset()
end

-- event wait lists
waitlists = {}
waitlists.events = {}
waitlists.frame = {}
waitlists.timer = {}

-- autograph.task.start
-- autograph.task.yield
-- autograph.task.waitForFrame()
-- autograph.task.waitForEvent(ev)
-- autograph.task.waitForSeconds(seconds)
-- task.onEvent() => internal
-- task.onFrame() => internal
-- autograph.onEvent(...) => internal
-- autograph.onFrame()

function waitForEvent()
	waitlists.events[coroutine.running()] = true
	return coroutine.yield()
end

function waitForFrame()
	waitlists.frame[coroutine.running()] = true
	return coroutine.yield()
end

-- called on reset
function reset()
	viewWidth, viewHeight = window:getFramebufferSize()
	scene = Scene2D()
	scene:loadTilemap('data/level1')
	-- processes
	procBrush = coroutine.create(brush)
	coroutine.resume(procBrush)
end

-- called when the window has been resized
function resize(w, h)
	ag.debug('resize: %i,%i', w, h)
	viewWidth = w
	viewHeight = h
end

-- resource reload request
function reload()

end


-- called when the frame is being rendered
function render(dt)
	--ag.debug('render (%ix%i), value=%f', framebufferWidth, framebufferHeight, value0)
    if window:getKey(ag.KEY_LEFT) == ag.KeyState.Pressed then
      viewX = viewX - moveSpeed * dt
    elseif window:getKey(ag.KEY_RIGHT) == ag.KeyState.Pressed then
      viewX = viewX + moveSpeed * dt
    elseif window:getKey(ag.KEY_UP) == ag.KeyState.Pressed then
      viewY = viewY - moveSpeed * dt
    elseif window:getKey(ag.KEY_DOWN) == ag.KeyState.Pressed then
       viewY = viewY + moveSpeed * dt
    end

    local shakeX = 0
    local shakeY = 0
    -- screen shake
    if window:getKey(ag.KEY_SPACE) == ag.KeyState.Pressed then
    	shakeX, shakeY = ag.diskRandom(shakeIntensity)
		--shakeIntensity = shakeIntensity + 0.1
	else 
		--shakeIntensity = 1.0
		shakeX, shakeY = 0, 0
	end

	scene:render(
		ag.getDefaultFramebuffer(),
		math.floor(viewX+shakeX), 
		math.floor(viewY+shakeY), 
		viewWidth/2, viewHeight/2)
end

function onEvent(ev) 
	--ag.debug('Got an event')
	-- resume all coroutines waiting on events
	for k,v in pairs(waitlists.events) do
		waitlists.events[k] = false
		coroutine.resume(k, ev)
	end
end

function brush()
		--ag.debug('Enter brush')
	while true do
		repeat 
			local ev = waitForEvent()
		until isa(ev, ag.PointerEvent)
		--ag.debug('Got a pointer event')
	end
end
