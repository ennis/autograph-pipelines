-- TODO
local ffi = require("ffi")
local gl, glc, glu, glfw, glext = require('gl').libraries()

-- Effect pipeline FFI
ffi.cdef[[

typedef struct {
  uint32_t addrU;
  uint32_t addrV;
  uint32_t addrW;
  uint32_t minFilter;
  uint32_t magFilter;
} SamplerDesc;

typedef enum {
  Image1D = 0,
  Image2D,
  Image3D,
  ImageCubeMap,
  Image1DArray,
  Image2DArray,
  ImageCubeArray
} ImageDimensions;

typedef enum {
  R32G32B32A32_SFLOAT = 0,
  R16G16B16A16_SFLOAT,
  R32G32_SFLOAT,
  R32_SFLOAT,
  R8_UNORM,
  R8G8B8A8_UNORM,
  R8G8B8A8_SNORM,
  B10G11R11_UFLOAT_PACK32,
  D32_SFLOAT,
  Max
} ImageFormat;

typedef enum {
 Compute = 0, 
 Screen, 
 Geometry
} PassType;

typedef struct Pipeline Pipeline;
typedef struct Pass Pass;
typedef struct Sampler Sampler;
typedef struct Texture Texture;

Texture *FXCreateTexture2D(Pipeline *pPipeline, const char *name,
                             ImageFormat imgFmt, int width, int height,
                             int numMips);
Texture *FXCreateTexture3D(Pipeline *pPipeline, const char *name,
                             ImageFormat imgFmt, int width, int height,
                             int depth, int numMips);
Pass *FXCreatePass(Pipeline *pPipeline, const char *name,
                     PassType pass_type);
Sampler *FXCreateSampler(Pipeline *pPipeline, const char *name,
                           const SamplerDesc *desc);
void FXPassBindTexture(Pipeline *pPipeline, Pass *pass, int slot,
                       Texture *tex);
void FXPassBindTextureImage(Pipeline* pPipeline, Pass* pass, int slot, Texture* tex);
void FXPassBindSampler(Pipeline* pPipeline, Pass* pass, int slot, Sampler* sampler);
int FXGenerateTextureID();
]]



function Texture2D.new(params)
	texname = if not params.name then
		"texture" .. FXGenerateTextureID
	else 
		params.name
	end

	assert(params.width)
	assert(params.height) 

	return FXCreateTexture2D(
		pipeline, 
		texname, 
		params.format, 
		params.width, 
		params.height, 
		params.numMips or 1);
end 


function Texture3D.new(params)
	texname = if not params.name then
		"texture" .. FXGenerateTextureID
	else 
		params.name
	end

	assert(params.width)
	assert(params.height) 
	assert(params.depth) 

	return FXCreateTexture3D(
		pipeline, 
		texname, 
		params.format, 
		params.width, 
		params.height, 
		params.depth, 
		params.numMips or 1);
end 