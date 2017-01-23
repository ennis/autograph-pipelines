gl = require 'glapi'

local mesh2DLayout = {
	{ buffer = 0, type = gl.GL_FLOAT, size = 2, stride = 8, normalized = false },
	{ buffer = 0, type = gl.GL_FLOAT, size = 2, stride = 8, normalized = false }
}

local mesh3DLayout = {
	{ buffer = 0, type = gl.GL_FLOAT, size = 3, stride = 12, normalized = false },
		{ buffer = 0, type = gl.GL_FLOAT, size = 3, stride = 12, normalized = false },
		{ buffer = 0, type = gl.GL_FLOAT, size = 3, stride = 12, normalized = false },
		{ buffer = 0, type = gl.GL_FLOAT, size = 2, stride = 8, normalized = false }
}

local geometryPassBase =
{
	layout = mesh3DLayout,
	rasterizerState = {
		fillMode = gl.GL_FILL,
		cullMode = gl.GL_BACK,
		frontFace = gl.GL_CCW
	},
	depthStencilState = {
		depthTestEnable = true,
		depthWriteEnable = true
	},
	blendState = {
		[0] = { 
			enabled = true,
			modeRGB = gl.GL_FUNC_ADD,
			modeAlpha = gl.GL_FUNC_ADD,
			funcSrcRGB = gl.GL_SRC_ALPHA,
			funcDstRGB = gl.GL_ONE_MINUS_SRC_ALPHA,
			funcSrcAlpha = gl.GL_ONE,
			funcDstAlpha = gl.GL_ZERO
		},
		[1] = { enabled = false }
	},
}


local geometry2DPassBase =
{
	layout = mesh2DLayout,
	rasterizerState = {
		fillMode = gl.GL_FILL,
		cullMode = gl.GL_BACK,
		frontFace = gl.GL_CCW
	},
	depthStencilState = {
		depthTestEnable = true,
		depthWriteEnable = true
	},
	blendState = {
		[0] = { 
			enabled = true,
			modeRGB = gl.GL_FUNC_ADD,
			modeAlpha = gl.GL_FUNC_ADD,
			funcSrcRGB = gl.GL_SRC_ALPHA,
			funcDstRGB = gl.GL_ONE_MINUS_SRC_ALPHA,
			funcSrcAlpha = gl.GL_ONE,
			funcDstAlpha = gl.GL_ZERO
		},
		[1] = { enabled = false }
	},
}

local screenPassBase = {
	layout = mesh2DLayout,
	rasterizerState = {
		fillMode = gl.GL_FILL,
		cullMode = gl.GL_BACK,
		frontFace = gl.GL_CCW
	},
	blendState = {
		[0] = { 
			enabled = true,
			modeRGB = gl.GL_FUNC_ADD,
			modeAlpha = gl.GL_FUNC_ADD,
			funcSrcRGB = gl.GL_SRC_ALPHA,
			funcDstRGB = gl.GL_ONE_MINUS_SRC_ALPHA,
			funcSrcAlpha = gl.GL_ONE,
			funcDstAlpha = gl.GL_ZERO
		},
		[1] = { enabled = false }
	},	
	depthStencilState = {
		depthTestEnable = false,
		depthWriteEnable = false
	},
}


function deepcopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in next, orig, nil do
            copy[deepcopy(orig_key)] = deepcopy(orig_value)
        end
        setmetatable(copy, deepcopy(getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end

function ScreenPass(table) 
	local tbl = deepcopy(screenPassBase)
	for k,v in pairs(table) do
		tbl[k] = v
	end
	return tbl
end

function GeometryPass(table) 
	local tbl = deepcopy(geometryPassBase)
	for k,v in pairs(table) do
		tbl[k] = v
	end
	return tbl
end

function Geometry2DPass(table) 
	local tbl = deepcopy(geometry2DPassBase)
	for k,v in pairs(table) do
		tbl[k] = v
	end
	return tbl
end

function ComputeShader(table)
	local tbl = {}
	tbl.isCompute = true
	for k,v in pairs(table) do
		tbl[k] = v
	end
	return tbl
end
