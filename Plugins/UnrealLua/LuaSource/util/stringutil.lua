StringUtil = {}
function StringUtil.ReplaceEscapedCharWithChar(str)
	return string.gsub(str, "\\n", "\n")
end

function StringUtil.Trim(str)
	return (str:gsub("^%s*(.-)%s*$", "%1"))
end

function StringUtil.Split(str, sep)
	if str == nil then return nil end
	local str1, str2 = string.match(str, "(.-)"..sep.."(.*)")
	if str1 == nil then return str end
	return str1, StringUtil.Split(str2, sep)
end


function StringUtil.tostring(t)
	local mark   = {}
	local assign = {}
 
	local function serialize(tbl, parent)
		mark[tbl] = parent
		local tmp = {}
		for k, v in pairs(tbl) do
			local typek = type(k)
			local typev = type(v)

			local key = typek == "number" and "[" .. k .."]" or k

			if typev == "table" then
				local dotkey = parent .. (typek == "number" and key or "." .. key)
				if mark[v] then
					table.insert(assign, dotkey .. "=" .. mark[v])
				else
					if typek == "number" then
						table.insert(tmp, serialize(v,dotkey))
					else
						table.insert(tmp, key .. "=" .. serialize(v, dotkey))
					end
				end
			else
				if typev == "string" then
					v = string.gsub(v, "\n", "\\n")
					if string.match( string.gsub(v,"[^'\"]",""), '^"+$' ) then
						v = "'" .. v .. "'"
					else
						v = '"' .. v .. '"'
					end
				else
					v = tostring(v)
				end

				if typek == "number" then
					table.insert(tmp, v)
				else
					table.insert(tmp, key .. "=" .. v)
				end
			end
		end
		return "{" .. table.concat(tmp, ",") .. "}"
	end
 
	return serialize(t, "ret") .. table.concat(assign," ")
end

function StringUtil.loadstring(str)
	local chunk = loadstring("do local ret = " .. str .. " return ret end")
	if chunk then
		return chunk()
	end
end

local FText_cached = {}
function NSLOCTEXT(NameSpace, Key, Value)
	local NameMap = FText_cached[NameSpace]
	if NameMap == nil then
		NameMap = {}
		FText_cached[NameSpace] = NameMap	
	end
	local ResultText = NameMap[Key]
	if not ResultText then
		ResultText = FText.NsLocText(NameSpace, Key, Value)
		NameMap[Key] = ResultText
	end
	return ResultText
end

function LOCTEXT(Key, Value)
	return NSLOCTEXT("", Key, Value)
end

return StringUtil