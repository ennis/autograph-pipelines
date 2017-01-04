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
  D32_SFLOAT = 8,
  A2R10G10B10_UNORM_PACK32 = 9,
  A2R10G10B10_SNORM_PACK32 = 10,
  R8_SRGB = 11,
  R8G8_SRGB = 12,
  R8G8B8_SRGB = 13,
  R8G8B8A8_SRGB = 14
}

autograph.EventType = {
    MouseButton = 0,
    MouseMove = 1,
    Cursor = 2,
    CursorEnter = 3,
    CursorExit = 4,
    MouseScroll = 5,
    Key = 6,
    Text = 7,
    StylusProximity = 8,
    StylusProperties = 9,
    WindowResize = 10
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

local function dump(o)
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

local function file_exists(name)
   local f=io.open(name,"r")
   if f~=nil then io.close(f) return true else return false end
end

function autograph.getTexture(textureId, format)
  format = format or autograph.ImageFormat.R8G8B8A8_SRGB
  autograph.loadedTextures = autograph.loadedTextures or {}
  local tex = autograph.loadedTextures[textureId]
  if tex then 
    autograph.debug("texture %s already loaded", textureId)
    return tex 
  end

  autograph.debug("loading texture %s", textureId)
  tex = autograph.loadTexture(textureId, format)
  autograph.loadedTextures[textureId] = tex
  return tex
end

return autograph
