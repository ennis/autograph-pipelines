local ffi = require 'ffi'
local types = {}

-- Effect pipeline FFI
ffi.cdef [[
	typedef struct {
		float x;
		float y;
	} vec2;

	typedef struct {
		float x;
		float y;
		float z;
	} vec3;

	typedef struct {
		float x;
		float y;
		float z;
		float w;
	} vec4;
]]

types.vec2 = ffi.typeof('vec2')
types.vec3 = ffi.typeof('vec3')
types.vec4 = ffi.typeof('vec4')

return types