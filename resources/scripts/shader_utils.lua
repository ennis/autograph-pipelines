-- we need the main bindings
require 'autograph'

shader_utils = shader_utils or {}

-- lua shader preprocessor
local function preprocess(filename, env)
  --autograph.debug('Preprocessing %s', filename)
  --autograph.debug('Environment:')
  --autograph.debug(dump(env))
  local file = io.open(filename)
  local chunk = {'local T={}\n'}
  for line in file:lines() do
    -- handle includes
     local filename = string.match(line, '#include "([%w_%.]+)"')
     if filename then
        local shaderFile = autograph.getActualPath('resources/shaders/' .. filename)
        local prep = preprocess(shaderFile, env)
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
    return nil
  else  
    env.table = table
    setfenv(chunkfn, env)
    str = chunkfn()
    --autograph.debug('Preprocessed source:')
    -- autograph.debug(str)
    return str
  end
end

function shader_utils.createShaderFromTemplate(shaderId, defines)
  local shader = _G[shaderId]
  if not shader then
    autograph.error('createShaderFromTemplate: shader %s undefined', shaderId)
    return nil
  end 
  local shaderFile = autograph.getActualPath('resources/shaders/' .. shader.shaderFile)
  defines._VERTEX_ = true
  local vs = preprocess(shaderFile, defines)
  defines._VERTEX_ = nil
  defines._FRAGMENT_ = true
  local fs = preprocess(shaderFile, defines)
  defines._FRAGMENT_ = nil
  
  local pass = table.shallow_copy(shader)
  pass.vertexShader = vs
  pass.fragmentShader = fs
  return pass
end

function shader_utils.createShaderPass(shaderId, defines)
  local builder = autograph.DrawPassBuilder()
  autograph.debug('autograph.createShaderPass(%s,...)', shaderId)
  builder:loadFromTable(createShaderFromTemplate(shaderId, defines))
  return builder:makeDrawPass()
end
