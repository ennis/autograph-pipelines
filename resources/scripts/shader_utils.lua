-- we need the main bindings
require 'autograph'

shader_utils = shader_utils or {}

local function printFileStack(filestack)
  autograph.error('Include stack:')
  for i = 1, #filestack do
    autograph.error('... %s', filestack[#filestack-i+1])
  end 
end

-- lua shader preprocessor
local function preprocess(filename, env, filestack)
  --autograph.debug('Preprocessing %s', filename)
  --autograph.debug('Environment:')
  --autograph.debug(dump(env))
  local file = io.open(filename)
  if not file then
    autograph.error('preprocess(): could not open file %s', filename)
    printFileStack(filestack)
    return nil
  end
  filestack[#filestack+1] = filename
  local chunk = {'local T={}\n'}
  for line in file:lines() do
    -- handle includes
     local filename = string.match(line, '#include "([%w_%.]+)"')
     if filename then
        local shaderFile = autograph.getActualPath('resources/shaders/' .. filename)
        local prep = preprocess(shaderFile, env, filestack)
        table.insert(chunk, string.format('table.insert(T,%q) ', prep))
     else
        local found = string.find(line, '##')
       if found then
        table.insert(chunk, string.sub(line, found+2) .. "\n")
       else
        local last = 1
        for text, expr, index in string.gmatch(line, "(.-)$(%b())()") do 
          last = index
          if text ~= "" then
            table.insert(chunk, string.format('table.insert(T,%q) ', text))
          end
          table.insert(chunk, string.format('table.insert(T,tostring %s) ', expr))
        end
        table.insert(chunk, string.format('table.insert(T,%q)\n',
                                           string.sub(line, last).."\n"))
      end
    end
  end
  file:close()
  table.insert(chunk, 'return table.concat(T)')
  local str = table.concat(chunk)
  local chunkfn, err = loadstring(str)
  if err then 
    autograph.error('Error while processing Lua shader template:')
    autograph.error('%s', err) 
    printFileStack(filestack)
  else  
    env.table = table
    setfenv(chunkfn, env)
    str = chunkfn()
    --autograph.debug('Preprocessed source:')
    -- autograph.debug(str)
  end

  filestack[#filestack] = nil
  return str
end

local function tableConcat(t1,t2)
    for i=1,#t2 do
        t1[#t1+1] = t2[i]
    end
    return t1
end

function shader_utils.createShaderFromTemplate(shaderId, defines)
  local shader = _G[shaderId]
  if not shader then
    autograph.error('createShaderFromTemplate: shader %s undefined', shaderId)
    return nil
  end 
  local shaderFile = autograph.getActualPath('resources/shaders/' .. shader.shaderFile)
  local pass = table.shallow_copy(shader)

  local shaderdefs = shader.defines
  if shaderdefs then
    for i=1,#shaderdefs do
      defines[#defines+1] = shaderdefs[i]
    end
  end

  if shader.isCompute then
    defines._COMPUTE_ = true
    local cs = preprocess(shaderFile, defines, {})
    pass.vertexShader = cs
  else
    defines._VERTEX_ = true
    local vs = preprocess(shaderFile, defines, {})
    defines._VERTEX_ = nil
    defines._FRAGMENT_ = true
    local fs = preprocess(shaderFile, defines, {})
    defines._FRAGMENT_ = nil
    pass.vertexShader = vs
    pass.fragmentShader = fs
  end
    
  return pass
end

