lcpp = require 'lcpp'

local gl = {}

gl.Texture = __bindings.Texture
--gl.Sampler = __bindings.Sampler
gl.Buffer = __bindings.Buffer
gl.Framebuffer = __bindings.Framebuffer

local function readToString(filepath)
    local f = io.open(filepath, "rb")
    local content = f:read("*all")
    f:close()
    return content
end

-- load a single-source shader file
gl.preprocessShaderSource = function (source) 
	local VS = lcpp.compile(str, {_VERTEX_=1})
	local FS = lcpp.compile(str, {_FRAGMENT_=1})
	return VS,FS
end

gl.preprocessShaderFile = function (file)
	local str = readToString(filepath)
	return gl.preprocessShaderSource(str)
end

return gl
