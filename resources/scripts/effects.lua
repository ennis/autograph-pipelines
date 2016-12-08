local ffi = require 'ffi'

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
]]
