autograph = autograph_bindings

-- Add helpers to the builtin autograph table
lcpp = require 'lcpp'

autograph.debug = function(msg, ...) autograph.debugMessage(string.format(msg, ...)) end
autograph.warning = function(msg, ...) autograph.warningMessage(string.format(msg, ...)) end
autograph.error = function(msg, ...) autograph.errorMessage(string.format(msg, ...)) end

autograph.loadMesh = function (id)
	local fullpath = autograph.getActualPath(id)
	local mesh = autograph.Mesh.loadFromFile(fullpath)
	autograph.debug('loading mesh %s (full path %s)', id, fullpath)
	return mesh
end 

autograph.ImageFormat = {
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

local function readToString(filepath)
    local f = io.open(filepath, "rb")
    local content = f:read("*all")
    f:close()
    return content
end

-- load a single-source shader file
autograph.preprocessShaderSource = function (source) 
  local VS = lcpp.compile(str, {_VERTEX_=1})
  local FS = lcpp.compile(str, {_FRAGMENT_=1})
  return VS,FS
end

autograph.preprocessShaderFile = function (file)
  local str = readToString(filepath)
  return autograph.preprocessShaderSource(str)
end

autograph.createDrawPass =  function(params)
  local builder = autograph.DrawPassBuilder()
  if params.rasterizerState then 
    builder:setRasterizerState(params.rasterizerState)
  end
  if params.depthStencilState then
    builder:setDepthStencilState(params.depthStencilState)
  end
  if params.blendState then 
    for key, bs in pairs(params.blendState) do
      builder:setBlendState(key, bs)
      end
  end
  if params.viewports then 
    for key, vp in pairs(params.viewports) do
      builder:setViewport(key, vp.x, vp.y, vp.w, vp.h)
    end
  end
  builder:setVertexShader(params.vertexShader)
  builder:setFragmentShader(params.fragmentShader)
  return builder:makeDrawPass()
end

return autograph