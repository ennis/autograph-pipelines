local fx = require 'fxcommon'
local samplers = require 'samplers'

-- Declare resources
local shadowMap = fx.Texture2D.new { 
	format = C.R8G8B8A8_UNORM, 
	width = 2048, 
	height = 2048 }

-- Create a pass
local blurH = fx.ComputePass.new {
    textures = { [0] = sm },
    samplers = { [0] = samplers.linearClamp }
}

