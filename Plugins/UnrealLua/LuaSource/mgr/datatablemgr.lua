DataTableMgr = Inherit(Singleton)

function DataTableMgr:Ctor()
	self.m_Cached = {}
end

--! get a DataTableForLua ins, Interface is below
--! LuaDataTableHelper.h
--! FindRow:give a key, Get a RowStructData
--! GetAllRows:return a table,key is RowName, value is RowStructData
--! GetRowNames:get a array, 
--[[
	local data = DataTableMgr:Get():GetTable('/Game/test.test')
	local alldata = data:GetAllRows()
	local names = data:GetRowNames()
	local value = data:FindRow(100)
]]
function DataTableMgr:GetTable(Path)
	local DataTable = self.m_Cached[Path]
	if not DataTable then
		DataTable = ULuaDataTableHelper.GetDataTable(Path)
		assert(DataTable~=nil)
		self.m_Cached[Path] = DataTable
	end
	return DataTable
end

function DataTableMgr:GetTableById(Id)
	local DataTable = self.m_Cached[Id]
	if not DataTable then
		DataTable = ULuaDataTableHelper.GetDataTableById(Id)
		assert(DataTable~=nil)
		self.m_Cached[Id] = DataTable
	end
	return DataTable
end

return DataTableMgr
