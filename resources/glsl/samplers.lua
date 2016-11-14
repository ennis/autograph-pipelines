fx = require 'fxcommon'

local samplers = {}

-- Define commonly used samplers
samplers.linearClamp = fx.Sampler.new {
    addrU = glc.GL_CLAMP_TO_EDGE,
    addrV = glc.GL_CLAMP_TO_EDGE,
    addvW = glc.GL_CLAMP_TO_EDGE,
    minFilter = glc.GL_LINEAR,
    magFilter = glc.GL_LINEAR }

samplers.nearestClamp = fx.Sampler.new {
    addrU = glc.GL_CLAMP_TO_EDGE,
    addrV = glc.GL_CLAMP_TO_EDGE,
    addvW = glc.GL_CLAMP_TO_EDGE,
    minFilter = glc.GL_NEAREST,
    magFilter = glc.GL_NEAREST }

return samplers