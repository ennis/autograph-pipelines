local core = {}

core.vec2 = __bindings.vec2
core.vec3 = __bindings.vec3
core.vec4 = __bindings.vec4
core.ivec2 = __bindings.ivec2
core.ivec3 = __bindings.ivec3
core.ivec4 = __bindings.ivec4

-- TODO this shouldn't be in core
core.Scene = __bindings.Scene
core.Mesh = __bindings.Mesh
core.Camera = __bindings.Camera
core.Frustum = __bindings.Frustum
core.Transform = __bindings.Transform

core.debug = function(msg, ...) __bindings.debugMessage(string.format(msg, ...)) end
core.warning = function(msg, ...) __bindings.warningMessage(string.format(msg, ...)) end
core.error = function(msg, ...) __bindings.errorMessage(string.format(msg, ...)) end

core.loadMesh = function (id)
	local fullpath = core.getActualPath(id)
	local mesh = core.Mesh.loadFromFile(fullpath)
	core.debug('loading mesh %s (full path %s)', id, fullpath)
	return mesh
end 

core.ImageFormat = {
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

core.getActualPath = __bindings.getActualPath
core.getProjectRootDirectory = __bindings.getProjectRootDirectory


return core