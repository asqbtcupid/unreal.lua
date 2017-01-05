sqllike = {}
sqllike._funcache = {}

function sqllike:from(...)
	local tables = {...}
	self._result = clone(tables[1])
	local i = 2
	while tables[i] do
		if type(tables[i+1]) == "table" then
			self:join(tables[i], tables[i+1])
			i = i + 2
		else
			self:join(tables[i], tables[i+2], tables[i+1])
			i = i + 3
		end
	end
	return self
end
function sqllike:join(foreignkeykey, jointable, newcolname)
	local newvalue = {}
	result = self._result
	for k, result_item in pairs(result) do
		if type(result_item) == "table" then
			local foreignkey = result_item[foreignkeykey]
			if foreignkey then
				local item = jointable[foreignkey]
				if type(item) == "table" then
					for k, v in pairs(item) do
						local newkey = k
						if newcolname then
							newkey = newcolname.."_"..k
						end
						result_item[newkey] = clone(v)
					end
				else
					result_item[newcolname or foreignkeykey] = item
				end
			end
		elseif k == foreignkeykey then
			newvalue[k] = clone(jointable[result_item])
		end
	end
	for k, v in pairs(newvalue) do
		result[k] = v
	end
	return self
end

function sqllike:where(condition_str)
	local fun = loadstring("return function (k, v)\n return "..condition_str.."\nend")()
	local keystonil = {}
	for k, v in pairs(self._result) do
		if not fun(k,v) then
			keystonil[k] = true
		end
	end
	for k, v in pairs(keystonil) do
		self._result[k]=nil	
	end
	return self
end

function sqllike:result()
	if self._select then
		local result = {}
		for i, v in pairs(self._result) do
			if type(v) == "table" then
				result[i] = {}
				for key, value in pairs(v) do
					if self._select[key] then
						result[i][key] = value
					end
				end
			elseif self._select[i] then
				result[i] = v 
			end
		end
		self._result = result
	end
	return self._result, self._result_key
end

function sqllike:arr()
	local result = {}
	local result_key = {}
	local len
	for k, v in pairs(self._result) do
		len = #result+1
		result[len] = v
		result_key[len] = k
	end
	self._result = result
	self._result_key = result_key
	return self
end

local function generatecode(key, op, constant)
	local code
	local nodot = false
	if tonumber(key) then
		nodot = true
		key = "["..key.."]"
	end
	if op ~= "==" then
		if key == "_k" then
			code = "if (a[1] ~= b[1] ) then return a[1] _op_ b[1] end\n"
		else
			code = "if (a[2].key ~= b[2].key ) then return a[2].key _op_ b[2].key end\n"
			code = code:gsub("key", key)
		end
		code = code:gsub("_op_", op)
	else
		if key == "_k" then
			code = "if (a[1] ~= b[1]) then if (a[1] == constant) then return true elseif (b[1] == constant) then return false end end\n"
		else
			code = "if (a[2].key ~= b[2].key ) then if (a[2].key == constant) then return true elseif (b[2].key == constant) then return false end end\n"
			code = code:gsub("key", key)
		end
		code = code:gsub("constant", constant)
	end
	if nodot then
		code = code:gsub("%.", "")
	end
	return code
end

function sqllike:sort(sortf)
	local result = self._result
	local bconsider_v = false
	for k, v in pairs(result) do
		if type(v) ~= "table" then
			bconsider_v = true
		end
		break
	end
	result = {}
	for k, v in pairs(self._result) do
		result[#result+1] = {k, v}
	end
	if type(sortf) == "string" then
		if not sqllike._funcache[sortf] then
			local funcstr = "return function(a, b)\n"
			for k, v in ipairs(SimpleDlg:Split(sortf, ",")) do
				local code
				local key, constant = v:match("(.*)<")
				if key then
					code = generatecode(key, "<")
				end
				key = v:match("(.*)>")
				if key then
					code = generatecode(key, ">")
				end
				key, constant = v:match("(.*)==(.*)")
				if key then
					code = generatecode(key, "==", constant)
				end
				funcstr = funcstr .. code
			end
			funcstr = funcstr .. "end"
			if bconsider_v then
				funcstr = funcstr:gsub("._v", "")
			end
			sqllike._funcache[sortf] = loadstring(funcstr)()
		end
		table.sort(result, sqllike._funcache[sortf])
	else
		table.sort(result, sortf)
	end
	self._result_key = {}
	self._result = {}
	for k, v in ipairs(result) do
		self._result[k] = v[2]
		self._result_key[k] = v[1]
	end
	return self
end

function sqllike:__index(k)
	return sqllike[k]
end

function sqllike:select(cols)
	self._select = {}
	for k, v in pairs(cols) do
		self._select[v] = true
	end
	return self
end
-- 类似sql的写法 
return sqllike