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
	{"nodeid", 4, "UCETN", data_type.C_String, 0},
	{"flag", 4, "UCETN", data_type.C_String, 0},
	{"counter", 4, "UCETN", data_type.C_String, 0},
	{"segment", 4, "UCETN", data_type.C_String, 0},
	{"nodetype", 4, "UCETN", data_type.DStr2DINum, 0},
	{"address", 4, "UCETN", data_type.C_String, 0},
	{"way1t", 6, "UCETN", data_type.DStr2DINum, 0},
	{"way1h", 4, "UCETN", data_type.HStr2DINum, 0},
	{"saltv", 4, "UCETN", data_type.DStr2DINum, 0},
	{"salts", 4, "UCETN", data_type.HStr2DINum, 0},
	{"remain", 22, "UCETN", data_type.C_String, 0},
}

packet = 
{
----{attribute key, 字段长度, 字段值, 数据类型, have key?}
	{"start_code", 4, "7E45", data_type.C_String, 0},
	{"hardware", 16, "UCETN", data_type.C_String, 0},
	{"data_code", 64, "data_field", data_type.UCETN, 0},
	{"hardware", 6, "UCETN", data_type.C_String, 0},
	{"end_code", 2, "7E", data_type.C_String, 0},
}
-------------------------------------------------------------------------------------------------
-- the format of serial packet, end
-------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------
-- the area of user's function, begin
-------------------------------------------------------------------------------------------------
A = 
{
	{0.000157, 0.000157, 0.000157},
}
Z = 
{
	{3000, 3000, 3000},
}
function resoSLPacket(pac_kv, ...)
	local node = lgateway.lresoSLPacket(pac_kv, "nodetype", 1, 4)

	local way1t = lgateway.lresoSLPacket(pac_kv, "way1t", 3, 4)
	way1t = lsensor.lexTemperature(way1t)
	neg = string.sub(lgateway.lgetFieldValue(pac_kv, "way1t"), 1, 2)
	if neg == "00" then
		way1t = 0-way1t
	end
	local way1h = lgateway.lresoSLPacket(pac_kv, "way1h", 1, 4)
	way1h = lsensor.lexHumidity(way1h, A[1][1], Z[1][1])

	local saltv = lgateway.lresoSLPacket(pac_kv, "saltv", 1, 4)

	local salts = lgateway.lresoSLPacket(pac_kv, "salts", 1, 4)

	-- local data = string.format("way1t=%.1f,way1h=%.2f%%;way2t=%.1f,way2h=%.2f%%;way3t=%.1f,way3h=%.2f%%;",
	-- 	way1t, way1h*100, way2t, way2h*100, way3t, way3h*100)

	local data = string.format("SensorNO=%d;4_a01001-Rtd=%.2f,4_a01001-Flag=N;4_a01002-Rtd=%.2f,4_a01002-Flag=N;4_w01008_v-Rtd=%.2f,4_w01008_v-Flag=N;4_w01008_s-Rtd=%.2f,4_w01008_s-Flag=N", 
		node, way1t, way1h, saltv, salts)

	return data
end