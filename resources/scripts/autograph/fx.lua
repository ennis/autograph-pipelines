local fx = {}

fx.DrawPass = __bindings.DrawPass

fx.DrawPassBuilder = __bindings.DrawPassBuilder

fx.createDrawPass =  function(params)
	local builder = fx.DrawPassBuilder()
	if params.rasterizerState then 
		builder:setRasterizerState(params.rasterizerState)
	end
	if params.depthStencilState then
		builder:setDepthStencilState(params.depthStencilState)
	end
	if params.blendState then 
		for key, bs in pairs(params.blendState) do
			builder:setBlendState(key, bs)
	    end
	end
	if params.viewports then 
		for key, vp in pairs(params.viewports) do
			builder:setViewport(key, vp.x, vp.y, vp.w, vp.h)
		end
	end
	builder:setVertexShader(params.vertexShader)
	builder:setFragmentShader(params.fragmentShader)
	return builder:makeDrawPass()
end

return fx