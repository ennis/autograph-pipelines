gl = require 'gl'
core = require 'core'
types = require 'types'

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

function loadMesh(id)
	local fullpath = core.getActualPath(id)
	local mesh = core.Mesh.loadFromFile(fullpath)
	print(string.format('loading mesh %s (full path %s)', id, fullpath))
	return mesh
end 

local tex2 = gl.Texture.create2D(ImageFormat.R32G32_SFLOAT, 1024, 1024, 1)
local mesh = loadMesh('resources/meshes/hogarth.obj')

function init() 
	print('init!')
	collectgarbage()
	local tex = gl.Texture.create2D(ImageFormat.R8G8B8A8_UNORM, 1024, 1024, 1)
	tex:reset()
	print(string.format('mesh AABB %f, %f, %f, %f, %f, %f', 
		mesh.AABB.xmin, mesh.AABB.xmax,
		mesh.AABB.ymin, mesh.AABB.ymax,
		mesh.AABB.zmin, mesh.AABB.zmax ))
end

function onRender()
	print(string.format('onRender (%ix%i)', screen_width, screen_height))
	core.drawMesh(mesh, pipeline, { uniforms = {
		screen_size = types.vec2(screen_width, screen_height)
		} })
end