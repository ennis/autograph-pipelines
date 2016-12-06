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

local scene = core.Scene()
local obj = scene:loadMesh(core.getActualPath('resources/meshes/hogarth.obj'))
--local renderer = core.SceneRenderer()


function init() 
	obj.transform:setPosition(0.0, 1.0, 0.0)
	print(string.format('tr.position = %f,%f,%f', obj.transform.position.x, obj.transform.position.y, obj.transform.position.z))
end

function onRender()
	print(string.format('onRender (%ix%i)', screen_width, screen_height))
	-- sceneRenderer.render()

	--core.drawMesh(mesh, pipeline, { uniforms = {
	--	screen_size = types.vec2(screen_width, screen_height)
	--	} })
end

-- issue: holding a reference to a mesh that is created in lua and referenced in a scene
-- once the reference is given to the scene (c++ side), the object on the Lua side can be garbage-collected, but it shouldn't
-- => use shared_ptr? 

-- hot-reloading of scripts?
-- invalidates all refs to meshes, textures, etc.