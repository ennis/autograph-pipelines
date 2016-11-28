ag = require 'gl'

local ImageFormat = {
  R32G32B32A32_SFLOAT = 0,
  R16G16B16A16_SFLOAT = 1,
  R32G32_SFLOAT = 2,
  R32_SFLOAT = 3,
  R8_UNORM = 4,
  R8G8B8A8_UNORM = 5,
  R8G8B8A8_SNORM = 6,
  B10G11R11_UFLOAT_PACK32 = 7,
  D32_SFLOAT = 8
}

function init() 
	print('init!')
	tex = ag.Texture.create2D(ImageFormat.R8G8B8A8_UNORM, 1024, 1024, 1)
	tex:reset()
end