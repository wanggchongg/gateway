------------------------------------------------------------------------------------------------
-- the area of system's settings, begin
------------------------------------------------------------------------------------------------
data_type =
{
	--字符串型， 无符号整型， 有符号整型， 变长代码型， 不确定
	C_String=1, U_Integer=2, S_Integer=3, V_Code=4, DStr2DINum=5, HStr2DINum=6, HStr2DFStr=7, UCETN=-1,
}

CRC =
{
	CRC16=1, CRC32=2, NONE=0
}

check_attribute = --transmit the start_code,check_code,end_code's name to lib of gataway
{
	"start_code", "check_code", "end_code",
}
-------------------------------------------------------------------------------------------------
-- the area of system's settings, end
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
-- the format of serial packet, begin
-------------------------------------------------------------------------------------------------
command_type = 1 -- 1, only have attribute value; 2, have attribute key and value
space_mark_fd = nil
space_mark_kv = nil
space_mark_lr = nil
check_type = CRC.NONE

data_field =
{
----{attribute key, 字段长度, 字段值, 数据类型, have key?}
	{"nodetype", 2, "UCETN", data_type.DStr2DINum, 0},
	{"shock1", 4, "UCETN", data_type.HStr2DINum, 0},
	{"shock2", 4, "UCETN", data_type.HStr2DINum, 0},
	{"shock3", 4, "UCETN", data_type.HStr2DINum, 0},
	{"remain", 26, "UCETN", data_type.C_String, 0},
}

packet =
{
----{attribute key, 字段长度, 字段值, 数据类型, have key?}
	{"start_code", 4, "7E45", data_type.C_String, 0},
	{"nodeid", 4, "UCETN", data_type.C_String, 0},
	{"flag", 2, "UCETN", data_type.C_String, 0},
	{"data_code", 40, "data_field", data_type.UCETN, 0},
	{"end_code", 2, "7E", data_type.C_String, 0},
}
-------------------------------------------------------------------------------------------------
-- the format of serial packet, end
-------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------
-- the area of user's function, begin
-------------------------------------------------------------------------------------------------
function resoSLPacket(pac_kv, ...)
	local node = lgateway.lresoSLPacket(pac_kv, "nodetype", 1, 2)
	if node ~= 5 then
		return nil
	end

	local shock1 = lgateway.lresoSLPacket(pac_kv, "shock1", 1, 4)
	shock1 = lsensor.lexShock(shock1)

	local shock2 = lgateway.lresoSLPacket(pac_kv, "shock2", 1, 4)
	shock2 = lsensor.lexShock(shock2)

	local shock3 = lgateway.lresoSLPacket(pac_kv, "shock3", 1, 4)
	shock3 = lsensor.lexShock(shock3)


	-- local data = string.format("way1t=%.1f,way1h=%.2f%%;way2t=%.1f,way2h=%.2f%%;way3t=%.1f,way3h=%.2f%%;",
	-- 	way1t, way1h*100, way2t, way2h*100, way3t, way3h*100)

	local data = string.format("SensorNO=%d;4_shock1-Rtd=%.3f,4_shock1-Flag=N;4_shock2-Rtd=%.3f,4_shock2-Flag=N;4_shock3-Rtd=%.3f,4_shock3-Flag=N",
		node, shock1, shock2, shock3)

	return data
end