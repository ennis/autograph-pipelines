require 'fxcommon'

-- Declare resources
sm = Texture2D.new { 
	name = 'shadowMap'	-- giving it a name makes it accessible from the C++ side
	format = TextureFormat.R8G8B8A8_UNORM, 
	width = 2048, 
	height = 2048 })

