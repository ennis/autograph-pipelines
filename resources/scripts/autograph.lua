autograph = autograph_bindings

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

function table.shallow_copy(t)
  local t2 = {}
  for k,v in pairs(t) do
    t2[k] = v
  end
  return t2
end

function dump(o)
   if type(o) == 'table' then
      local s = '{ '
      for k,v in pairs(o) do
         if type(k) ~= 'number' then k = '"'..k..'"' end
         s = s .. '['..k..'] = ' .. dump(v) .. ','
      end
      return s .. '} '
   else
      return tostring(o)
   end
end


function preprocess(filename, env)
  autograph.debug('Preprocessing %s', filename)
  autograph.debug('Environment:')
  autograph.debug(dump(env))
  local file = io.open(filename)
  local chunk = {'local T={}\n'}
  for line in file:lines() do
     local found = string.find(line, '##')
     if found then
      table.insert(chunk, string.sub(line, found+2) .. "\n")
     else
      local last = 1
      for text, expr, index in string.gmatch(line, "(.-)$(%b())()") do 
        last = index
        if text ~= "" then
          table.insert(chunk, string.format('table.insert(T,%q) ', text))
        end
        table.insert(chunk, string.format('table.insert(T,tostring %s) ', expr))
      end
      table.insert(chunk, string.format('table.insert(T,%q)\n',
                                         string.sub(line, last).."\n"))
    end
  end
  file:close()
  table.insert(chunk, 'return table.concat(T)')
  local str = table.concat(chunk)
  local chunkfn, err = loadstring(str)
  if err then 
    autograph.error('Error while processing Lua shader template:')
    autograph.error('%s', err) 
    return nil
  else  
    env.table = table
    setfenv(chunkfn, env)
    str = chunkfn()
    autograph.debug('Preprocessed source:')
    autograph.debug(str)
    env.table = nil
    return str
  end
end

local ppCombinedShaderFile = function (file, defines)
  preprocess(file, defines)
end

-- autograph.createDrawPass =  function(params)
--   local builder = autograph.DrawPassBuilder()
--   if params.rasterizerState then 
--     builder:setRasterizerState(params.rasterizerState)
--   end
--   if params.depthStencilState then
--     builder:setDepthStencilState(params.depthStencilState)
--   end
--   if params.blendState then 
--     for key, bs in pairs(params.blendState) do
--       builder:setBlendState(key, bs)
--       end
--   end
--   if params.viewports then 
--     for key, vp in pairs(params.viewports) do
--       builder:setViewport(key, vp.x, vp.y, vp.w, vp.h)
--     end
--   end
--   builder:setVertexShader(params.vertexShader)
--   builder:setFragmentShader(params.fragmentShader)
--   return builder:makeDrawPass()
-- end


autograph.createShaderFromTemplate = function(shaderId, defines)
  local shader = _G[shaderId]
  local shaderFile = autograph.getActualPath('resources/shaders/' .. shader.shaderFile)
  --if shader.vertexShader then
  --  local vs = ppShaderSource(shader.vertexShader, defines)
  --end
  --if shader.fragmentShader then
  --  local fs = ppShaderSource(shader.fragmentShader, defines)
  --end
  --if shader.vertexShaderFile then
  --  local vs = ppShaderFile(shader.vertexShaderFile, defines)
  --end
  --if shader.fragmentShaderFile then
  --  local fs = ppShaderFile(shader.fragmentShaderFile, defines)
  --end
  defines._VERTEX_ = true
  local vs = preprocess(shaderFile, defines)
  defines._VERTEX_ = nil
  defines._FRAGMENT_ = true
  local fs = preprocess(shaderFile, defines)
  defines._FRAGMENT_ = nil
  
  local pass = table.shallow_copy(shader)
  pass.vertexShader = vs
  pass.fragmentShader = fs
  return pass
end

autograph.createShaderPass = function(shaderId, defines)
  local builder = autograph.DrawPassBuilder()
  autograph.debug('autograph.createShaderPass(%s,...)', shaderId)
  builder:loadFromTable(createShaderFromTemplate(shaderId, defines))
  return builder:makeDrawPass()
end

return autograph
