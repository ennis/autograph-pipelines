ag = require 'autograph'

local function jitter(ref, jitter)
	return ref + ag.linearRand(-jitter, jitter)
end

local pointerPositions = {}
local splatPositions = {}

local spacing = 1.0
local spacingJitter = 0.0

-- local function addPoint(x, y, callback)
-- 	local sp = jitter(spacing, spacingJitter)
-- 	if sp < 0.1 then sp = 0.1 end
-- 	if next(pointerPositions) == nil then 
-- 		table.insert(pointerPositions, { x = x, y = y})
-- 		table.insert(splatPositions, { x = x, y = y})
-- 		callback({x = x, y = y, first = true, prevX = 0, prevY = 0})
-- 	elseif 
-- 	  plast = pointerPositions_.back();
--       vec2 Pmouse{x, y};
--       vec2 Pcur = glm::mix(Plast, Pmouse, 0.5); // smoothing
--       auto length = glm::distance(Plast, Pcur);
--       auto slack = pathLength_;
--       pathLength_ += length;
--       auto pos = spacing - slack;
--       int n = 0;
--       auto Pprev = splatPositions_.back();
--       while (pathLength_ > spacing) {
--         auto P = glm::mix(Plast, Pcur, (length > 0.01f) ? pos / length : 0.0f);
--         splatPositions_.push_back(P);
--         f(Point{false, P.x, P.y, Pprev.x, Pprev.y});
--         Pprev = P;
--         ++n;
--         pathLength_ -= spacing;
--         pos += spacing;
--       }
--       pointerPositions_.push_back(Pmouse);
--       return n;
-- 	end
-- end


function brushProc() 
	local ev
	repeat 
		ev = waitForEvent()
	until isa(ev, ag.PointerDownEvent)
	-- 
	-- addPoint
	ag.debug("BEGIN STROKE")

	while true do 
		ev = waitForEvent()
		if isa(ev, ag.PointerMoveEvent) then 
			-- addPoint
			ag.debug("STROKE ADD POINT") 
		elseif isa(ev, ag.PointerUpEvent) then 
			-- addPoint and end stroke
			ag.debug("STROKE END")
			break
		end
	end
	
end