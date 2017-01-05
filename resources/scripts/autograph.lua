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

autograph.KEY_SPACE=              32
autograph.KEY_APOSTROPHE=         39  
autograph.KEY_COMMA=              44 
autograph.KEY_MINUS=              45  
autograph.KEY_PERIOD=             46  
autograph.KEY_SLASH=              47 
autograph.KEY_0=                  48
autograph.KEY_1=                  49
autograph.KEY_2=                  50
autograph.KEY_3=                  51
autograph.KEY_4=                  52
autograph.KEY_5=                  53
autograph.KEY_6=                  54
autograph.KEY_7=                  55
autograph.KEY_8=                  56
autograph.KEY_9=                  57
autograph.KEY_SEMICOLON=          59  
autograph.KEY_EQUAL=              61  
autograph.KEY_A=                  65
autograph.KEY_B=                  66
autograph.KEY_C=                  67
autograph.KEY_D=                  68
autograph.KEY_E=                  69
autograph.KEY_F=                  70
autograph.KEY_G=                  71
autograph.KEY_H=                  72
autograph.KEY_I=                  73
autograph.KEY_J=                  74
autograph.KEY_K=                  75
autograph.KEY_L=                  76
autograph.KEY_M=                  77
autograph.KEY_N=                  78
autograph.KEY_O=                  79
autograph.KEY_P=                  80
autograph.KEY_Q=                  81
autograph.KEY_R=                  82
autograph.KEY_S=                  83
autograph.KEY_T=                  84
autograph.KEY_U=                  85
autograph.KEY_V=                  86
autograph.KEY_W=                  87
autograph.KEY_X=                  88
autograph.KEY_Y=                  89
autograph.KEY_Z=                  90
autograph.KEY_LEFT_BRACKET=       91  
autograph.KEY_BACKSLASH=          92  
autograph.KEY_RIGHT_BRACKET=      93 
autograph.KEY_GRAVE_ACCENT=       96  
autograph.KEY_WORLD_1=            161 
autograph.KEY_WORLD_2=            162 
autograph.KEY_ESCAPE=             256
autograph.KEY_ENTER=              257
autograph.KEY_TAB=                258
autograph.KEY_BACKSPACE=          259
autograph.KEY_INSERT=             260
autograph.KEY_DELETE=             261
autograph.KEY_RIGHT=              262
autograph.KEY_LEFT=               263
autograph.KEY_DOWN=               264
autograph.KEY_UP=                 265
autograph.KEY_PAGE_UP=            266
autograph.KEY_PAGE_DOWN=          267
autograph.KEY_HOME=               268
autograph.KEY_END=                269
autograph.KEY_CAPS_LOCK=          280
autograph.KEY_SCROLL_LOCK=        281
autograph.KEY_NUM_LOCK=           282
autograph.KEY_PRINT_SCREEN=       283
autograph.KEY_PAUSE=              284
autograph.KEY_F1=                 290
autograph.KEY_F2=                 291
autograph.KEY_F3=                 292
autograph.KEY_F4=                 293
autograph.KEY_F5=                 294
autograph.KEY_F6=                 295
autograph.KEY_F7=                 296
autograph.KEY_F8=                 297
autograph.KEY_F9=                 298
autograph.KEY_F10=                299
autograph.KEY_F11=                300
autograph.KEY_F12=                301
autograph.KEY_F13=                302
autograph.KEY_F14=                303
autograph.KEY_F15=                304
autograph.KEY_F16=                305
autograph.KEY_F17=                306
autograph.KEY_F18=                307
autograph.KEY_F19=                308
autograph.KEY_F20=                309
autograph.KEY_F21=                310
autograph.KEY_F22=                311
autograph.KEY_F23=                312
autograph.KEY_F24=                313
autograph.KEY_F25=                314
autograph.KEY_KP_0=               320
autograph.KEY_KP_1=               321
autograph.KEY_KP_2=               322
autograph.KEY_KP_3=               323
autograph.KEY_KP_4=               324
autograph.KEY_KP_5=               325
autograph.KEY_KP_6=               326
autograph.KEY_KP_7=               327
autograph.KEY_KP_8=               328
autograph.KEY_KP_9=               329
autograph.KEY_KP_DECIMAL=         330
autograph.KEY_KP_DIVIDE=          331
autograph.KEY_KP_MULTIPLY=        332
autograph.KEY_KP_SUBTRACT=        333
autograph.KEY_KP_ADD=             334
autograph.KEY_KP_ENTER=           335
autograph.KEY_KP_EQUAL=           336
autograph.KEY_LEFT_SHIFT=         340
autograph.KEY_LEFT_CONTROL=       341
autograph.KEY_LEFT_ALT=           342
autograph.KEY_LEFT_SUPER=         343
autograph.KEY_RIGHT_SHIFT=        344
autograph.KEY_RIGHT_CONTROL=      345
autograph.KEY_RIGHT_ALT=          346
autograph.KEY_RIGHT_SUPER=        347
autograph.KEY_MENU=               348

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
