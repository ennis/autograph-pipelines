local ui = {}

-- core type: widget (w)
-- addRootWidget()
-- createButton()
-- createSlider()

local function table.clone(org)
  return {table.unpack(org)}
end


function ui.add(w) 
	return __bindings.addRootWidget(w)
end

function ui.button(params)
	w = table.clone(params)
	w.ptr = __bindings.createButton(params.label, params.onClick)
	return w
end

function ui.dock(params)
	w = table.clone(params)
	w.ptr = __bindings.createDock(params.name)
	-- add contents
	if params.contents then
		for k,v in pairs(params.contents) do
			__bindings.addChild(w.ptr, v)
		end
	end
	return w
end

function ui.slider(params)
	w = table.clone(params)
	w.ptr = __bindings.createSlider(params.label, params.min, params.max, function (value) w.value = value end )
	return w
end

return ui