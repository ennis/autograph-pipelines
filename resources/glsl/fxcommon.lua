-- TODO
local ffi = require('ffi')
gl, glc, glu, glfw, glext = require('gl').libraries()

local fxcommon = {}

-- Effect pipeline FFI
ffi.cdef [[

typedef struct {
  bool enabled;
  uint32_t modeRGB;
  uint32_t modeAlpha;
  uint32_t funcSrcRGB;
  uint32_t funcDstRGB;
  uint32_t funcSrcAlpha;
  uint32_t funcDstAlpha;
} BlendState;

typedef struct {
  bool depthTestEnable;
  bool depthWriteEnable;
  bool stencilEnable;
  uint32_t stencilFace;
  uint32_t stencilFunc;
  uint32_t stencilRef;
  uint32_t stencilMask;
  uint32_t stencilOpSfail;
  uint32_t stencilOpDPFail;
  uint32_t stencilOpDPPass;
} DepthStencilState;

typedef struct {
  uint32_t fillMode;
  uint32_t cullMode;
  uint32_t frontFace;
  float depthBias;
  float slopeScaledDepthBias;
  bool depthClipEnable;
  bool scissorEnable;
} RasterizerState;

typedef struct {
  int x;
  int y;
  int width;
  int height;
} ScissorRect;

typedef struct {
  uint32_t addrU;
  uint32_t addrV;
  uint32_t addrW;
  uint32_t minFilter;
  uint32_t magFilter;
} SamplerDesc;

typedef enum {
  ImageDimensions_1D = 0,
  ImageDimensions_2D,
  ImageDimensions_3D,
  ImageDimensions_CubeMap,
  ImageDimensions_1DArray,
  ImageDimensions_2DArray,
  ImageDimensions_CubeArray
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
 PassType_Compute = 0,
 PassType_Screen,
 PassType_Draw
} PassType;

typedef struct {
  bool red;
  bool green;
  bool blue;
  bool alpha;
} ColorMask;

typedef enum {
  BufferUsage_Upload = 0,     
  BufferUsage_Default, 
  BufferUsage_Readback
} BufferUsage;

typedef struct Pipeline Pipeline;
typedef struct Pass Pass;
typedef struct DrawPass DrawPass;
typedef struct ScreenPass ScreenPass;
typedef struct ComputePass ComputePass;
typedef struct Sampler Sampler;
typedef struct Texture Texture
typedef struct Buffer Buffer;
typedef struct DrawStates DrawStates;

Texture *Pipeline_CreateTexture2D(Pipeline *pPipeline, const char *name,
                             ImageFormat imgFmt, int width, int height,
                             int numMips);
Texture *Pipeline_CreateTexture3D(Pipeline *pPipeline, const char *name,
                             ImageFormat imgFmt, int width, int height,
                             int depth, int numMips);
Sampler *Pipeline_CreateSampler(Pipeline *pPipeline, const char *name,
                           const SamplerDesc *desc);
Buffer *Pipeline_CreateBuffer(Pipeline *pPipeline, const char *name,
                          BufferUsage usage, int size);

ComputePass *Pipeline_CreateComputePass(Pipeline *pPipeline, const char *name);
ScreenPass *Pipeline_CreateScreenPass(Pipeline *pPipeline, const char *name);
DrawPass *Pipeline_CreateDrawPass(Pipeline *pPipeline, const char *name);

void Pass_BindTexture(Pass *pass, int slot, Texture *tex);
void Pass_BindSampler(Pass *pass, int slot, Sampler* sam);
void Pass_BindTextureImage(Pass* pass, int slot, Texture* tex);

RasterizerState* DrawPass_GetRasterizerState(DrawPass* pass);
DepthStencilState* DrawPass_GetDepthStencilState(DrawPass* pass);
BlendState* DrawPass_GetBlendState(DrawPass* pass, int index);
Viewport* DrawPass_GetViewport(DrawPass* pass, int index);
void DrawPass_BindColorBuffer(DrawPass* pass, int index, Texture* tex);
void DrawPass_BindDepthBuffer(DrawPass* pass, Texture* tex);

void ComputePass_SetGlobalSize(ComputePass* pass, int x, int y, int z);

int Pipeline_GenerateTextureID();
]]


--------------------------------------------
--
--
--
fxcommon.C = ffi.C

--------------------------------------------
--
--
--
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

--------------------------------------------
--
--
--
function fxcommon.CreateTexture2D(params)
    params.name = params.name or "texture" .. C.Pipeline_GenerateTextureID()
    print(dump(params))
    assert(params.format)
    assert(params.width)
    assert(params.height)
    return C.Pipeline_CreateTexture2D(
    g_pipeline,
    params.name,
    params.format,
    params.width,
    params.height,
    params.numMips or 1)
end 

--------------------------------------------
--
--
--
function fxcommon.CreateTexture3D(params)
    params.name = params.name or "texture" .. C.Pipeline_GenerateTextureID()
    assert(params.format)
    assert(params.width)
    assert(params.height)
    assert(params.depth)
    return C.Pipeline_CreateTexture3D(
    g_pipeline,
    params.name,
    params.format,
    params.width,
    params.height,
    params.depth,
    params.numMips or 1)
end 

--------------------------------------------
--
--
--
function fxcommon.CreateSampler(params) 
    local samplerDesc = ffi.new("SamplerDesc")
    samplerDesc.addrU = params.addrU or glc.GL_CLAMP_TO_EDGE
    samplerDesc.addrV = params.addrV or glc.GL_CLAMP_TO_EDGE
    samplerDesc.addrW = params.addrW or glc.GL_CLAMP_TO_EDGE
    samplerDesc.minFilter = params.minFilter or glc.GL_NEAREST
    samplerDesc.magFilter = params.magFilter or glc.GL_NEAREST
    local name = params.name or "sampler" .. C.Pipeline_GenerateTextureID()
    return C.Pipeline_CreateSampler(g_pipeline, name, samplerDesc)
end

--------------------------------------------
--
--
--
local function BindShaderResources(pass, params)
   -- textures
    if params.textures then
    for slot, tex in pairs(params.textures) do
        C.Pipeline_BindTexture(g_pipeline, ffi.cast('Pass*',pass), slot, tex)
    end
  end

    -- samplers
    if params.samplers then
    for slot, sampler in pairs(params.samplers) do
        C.Pipeline_BindSampler(g_pipeline, ffi.cast('Pass*',pass), slot, sampler)
    end
  end

    -- images
    if params.images then
    for slot, tex in pairs(params.images) do
        C.Pipeline_BindTextureImage(g_pipeline, ffi.cast('Pass*',pass), slot, tex) 
    end
    end
end

--------------------------------------------
--
--
--
function fxcommon.CreateComputePass(params)
    params.name = params.name or "computePass" .. C.Pipeline_GenerateTextureID()
    local pass = C.Pipeline_CreateComputePass(g_pipeline, params.name)
    BindShaderResources(pass, params)
    return pass
end

--------------------------------------------
--
--
--
function fxcommon.CreateDrawPass(params)
  params.name = params.name or "drawPass" .. C.Pipeline_GenerateTextureID()
  local pass = C.Pipeline_CreateDrawPass(g_pipeline, params.name)
  BindShaderResources(pass, params)
  -- scissor rect
  if params.scissor then
    pass.scissorRect.x = params.scissor.x or 0
    pass.scissorRect.y = params.scissor.x or 0
    pass.scissorRect.width = params.scissor.x or 0
    pass.scissorRect.height = params.scissor.x or 0
  end
  -- rasterizer state
  local RS = C.DrawPass_GetRasterizerState(pass)
  if params.fillMode then RS.fillMode = params.fillMode end 
  if params.cullMode then RS.cullMode = params.cullMode end 
  if params.frontFace then RS.frontFace = params.frontFace end 
  if params.depthBias then RS.depthBias = params.depthBias end 
  if params.slopeScaledDepthBias then RS.slopeScaledDepthBias = params.slopeScaledDepthBias end 
  if params.depthClipEnable then RS.depthClipEnable = params.depthClipEnable end 
  if params.scissorEnable then RS.scissorEnable = params.scissorEnable end 
  -- depth stencil state
  local DSS = C.DrawPass_GetRasterizerState(pass)
  if params.depthTestEnable then DSS.depthTestEnable = params.depthTestEnable end
  if params.depthWriteEnable then DSS.depthWriteEnable = params.depthWriteEnable end
  if params.stencilEnable then DSS.stencilEnable = params.stencilEnable end
  if params.stencilFace then DSS.stencilFace = params.stencilFace end
  if params.stencilFunc then DSS.stencilFunc = params.stencilFunc end
  if params.stencilRef then DSS.stencilRef = params.stencilRef end
  if params.stencilMask then DSS.stencilMask = params.stencilMask end
  if params.stencilOpSfail then DSS.stencilOpSfail = params.stencilOpSfail end
  if params.stencilOpDPFail then DSS.stencilOpDPFail = params.stencilOpDPFail end
  if params.stencilOpDPPass then DSS.stencilOpDPPass = params.stencilOpDPPass end
  -- blend states
  if params.blendStates then
    for index, blendStateParams in pairs(params.blendStates) do
      local BS = C.DrawPass_GetBlendState(pass, index)
      if blendStateParams.enabled then BS.enabled = blendStateParams.enabled end
      if blendStateParams.modeRGB then BS.modeRGB = blendStateParams.modeRGB end
      if blendStateParams.modeAlpha then BS.modeAlpha = blendStateParams.modeAlpha end
      if blendStateParams.funcSrcRGB then BS.funcSrcRGB = blendStateParams.funcSrcRGB end
      if blendStateParams.funcDstRGB then BS.funcDstRGB = blendStateParams.funcDstRGB end
      if blendStateParams.funcSrcAlpha then BS.funcSrcAlpha = blendStateParams.funcSrcAlpha end
      if blendStateParams.funcDstAlpha then BS.funcDstAlpha = blendStateParams.funcDstAlpha end
    end
  end
  -- viewports
  if params.viewports then 
    for index, viewport in pairs(params.viewports) do
      local VP = C.DrawPass_GetViewport(pass, index)
      assert(viewport.x)
      assert(viewport.y)
      assert(viewport.width)
      assert(viewport.height)
      VP.x = viewport.x
      VP.y = viewport.y
      VP.width = viewport.width
      VP.height = viewport.height
    end
  end
  -- draw buffers
  if params.colorBuffers then 
    for index,tex in pairs(params.colorBuffers) do
      C.DrawPass_BindColorBuffer(pass, index, tex)
    end
  end
  if params.depthBuffer then 
    C.DrawPass_BindDepthBuffer(pass, params.depthBuffer)
  end
  
end

return fxcommon
