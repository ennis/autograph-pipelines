local ffi = require 'ffi'
local types = {}

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

	typedef struct {
		int x;
		int y;
	} ivec2;

	typedef struct {
		int x;
		int y;
		int z;
	} ivec3;

	typedef struct {
		int x;
		int y;
		int z;
		int w;
	} ivec4;

    // core.AABB
    typedef struct {
      float xmin;
      float ymin;
      float zmin;
      float xmax;
      float ymax;
      float zmax;
    } AABB;

    // core.Vertex
    typedef struct {
        vec3 position;
        vec3 normal;
        vec3 tangent;
        vec2 texcoords;
    } Vertex;
]]

-- metatable for vec2
-- local mt.vec2 = {
--   __add = function(a, b) return vec2(a.x+b.x, a.y+b.y) end,
--   __mul = function(a, b) return if type(a) == "number" then vec2(a*b.x, a*b.y) else if type(b) == "number" then vec2(a.x*b, a.y*b) else vec2(a.x*b.x, a.y*b.y) end end,
--   __div = function(a, b) return if type(a) == "number" then vec2(a/b.x, a/b.y) else if type(b) == "number" then vec2(a.x/b, a.y/b) else vec2(a.x/b.x, a.y/b.y) end end,
--   __index = {
--     normalize = function(a) return a / a:length() end,
--     length = function(a) return math.sqrt(a:lengthSq()) end,
--     lengthSq = function(a) return a.x*a.x + a.y*a.y end,
--   },
-- }

-- local mt.vec3 = {
--   __add = function(a, b) return vec3(a.x+b.x, a.y+b.y, a.z+b.z) end,
--   __mul = function(a, b) return if type(a) == "number" then vec3(a*b.x, a*b.y, a*b.z) else if type(b) == "number" then vec3(a.x*b, a.y*b, a.z*b) else vec3(a.x*b.x, a.y*b.y, a.z*b.z) end end,
--   __div = function(a, b) return if type(a) == "number" then vec3(a/b.x, a/b.y, a/b.z) else if type(b) == "number" then vec3(a.x/b, a.y/b, a.z/b) else vec3(a.x/b.x, a.y/b.y, a.z/b.z) end end,
--   __index = {
--     normalize = function(a) return a / a:length() end,
--     length = function(a) return math.sqrt(a:lengthSq()) end,
--     lengthSq = function(a) return a.x*a.x + a.y*a.y + a.z*a.z end,
--   },
-- }

-- local mt.vec4 = {
--   __add = function(a, b) return vec4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w) end,
--   __mul = function(a, b) return if type(a) == "number" then vec3(a*b.x, a*b.y, a*b.z, a*b.w) else if type(b) == "number" then vec3(a.x*b, a.y*b, a.z*b, a.w*b) else vec3(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w) end end,
--   __div = function(a, b) return if type(a) == "number" then vec3(a/b.x, a/b.y, a/b.z, a/b.w) else if type(b) == "number" then vec3(a.x/b, a.y/b, a.z/b, a.w*b) else vec3(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w) end end,
--   __index = {
--     normalize = function(a) return a / a:length() end,
--     length = function(a) return math.sqrt(a:lengthSq()) end,
--     lengthSq = function(a) return a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w end,
--   },
-- }

types.ivec2 = ffi.typeof('ivec2')
types.ivec3 = ffi.typeof('ivec3')
types.ivec4 = ffi.typeof('ivec4')
types.vec2 = ffi.typeof('vec2')
types.vec3 = ffi.typeof('vec3')
types.vec4 = ffi.typeof('vec4')

-- types.dot = function (a, b) 
-- 	if ffi.ctype(a, core.vec2) and ffi.ctype(b, core.vec2) then
-- 		return a.x*b.x + a.y*b.y
-- 	else if ffi.ctype(a, core.vec3) and ffi.ctype(b, core.vec3) then
-- 		return a.x*b.x + a.y*b.y + a.z*b.z
-- 	else if ffi.ctype(a, core.vec4) and ffi.ctype(b, core.vec4) then
-- 		return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w
--     else
--         error("incompatible types for types.dot") 
--     end
-- end 

-- C++: C wrapper fn
-- lua: ffi struct def
-- lua: ffi wrapper fn
-- lua: metatable
-- lua: metatype

-- VS: (usertype binding)
-- C++: bind type with sol2

-- solution: 
-- base types (values?) with FFI, all functions in Lua (no FFI call)
-- complex types (references?) with C++-side usertypes 
-- => essentially two APIs

-- issue: returning a ctype from a usertype method call (these calls do not go through the FFI)
-- these calls push an userdata on the stack: must convert this userdata to the corresponding ctype: usertype -> ctype
-- same when passing parameters: ctype -> usertype
-- usertype -> ctype will cast to void*

-- other solution:
-- do not use ctypes OR usertypes in bindings (except reference types)
-- provide two C++ versions: one with vectors, another with primitive types (floats) 

return types
