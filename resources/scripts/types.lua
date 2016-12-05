local ffi = require 'ffi'
local core = {}

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

	typedef enum {
	  R32G32B32A32_SFLOAT = 0,
	  R16G16B16A16_SFLOAT = 1,
	  R32G32_SFLOAT = 2,
	  R32_SFLOAT = 3,
	  R8_UNORM = 4,
	  R8G8B8A8_UNORM = 5,
	  R8G8B8A8_SNORM = 6,
	  B10G11R11_UFLOAT_PACK32 = 7,
	  D32_SFLOAT = 8
	} ImageFormat;

	// core.Texture
	typedef struct Texture;

	// core.Mesh
	typedef struct Mesh;

	// core.Vertex
	typedef struct {
 		vec3 position;
  		vec3 normal;
  		vec3 tangent;
  		vec2 texcoords;
	} Vertex;

]]

-- metatable for vec2
local mt = {
  __add = function(a, b) return vec2(a.x+b.x, a.y+b.y) end,
  __mul = function(a, b) return 
  				if type(a) == "number" then 
  						vec2(a*b.x, a*b.y)
  					else if type(b) == "number" then 
  						vec2(a.x*b, a.y*b)
  					else 
  						vec2(a.x*b.x, a.y*b.y) 
  					end
  			 end,
  __div = function(a, b) return 
  				if type(a) == "number" then 
  						vec2(a/b.x, a/b.y)
  					else if type(b) == "number" then 
  						vec2(a.x/b, a.y/b)
  					else 
  						vec2(a.x/b.x, a.y/b.y) 
  					end
  			 end,

  __index = {
    normalize = function(a) return a / math.sqrt(a.x*a.x + a.y*a.y) end,
  },
}

core.dot = function (a, b) 
	return 
end 

core.vec2 = ffi.typeof('vec2')
core.vec3 = ffi.typeof('vec3')
core.vec4 = ffi.typeof('vec4')
core.ivec2 = ffi.typeof('ivec2')
core.ivec3 = ffi.typeof('ivec3')
core.ivec4 = ffi.typeof('ivec4')
core.ImageFormat = ffi.typeof('ImageFormat')

return core