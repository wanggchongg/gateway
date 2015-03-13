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
	{"way2t", 6, "UCETN", data_type.DStr2DINum, 0},
	{"way2h", 4, "UCETN", data_type.HStr2DINum, 0},
	{"way3t", 6, "UCETN", data_type.DStr2DINum, 0},
	{"way3h", 4, "UCETN", data_type.HStr2DINum, 0},
	{"remain", 10, "UCETN", data_type.C_String, 0},
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
	-- local flag = lgateway.lgetFieldValue(pac_kv, "flag")
	-- if flag == "0001" then
	-- 	return nil
	-- end

	local node = lgateway.lresoSLPacket(pac_kv, "nodetype", 1, 4)
	
	local way1t = lgateway.lresoSLPacket(pac_kv, "way1t", 3, 4)
	way1t = lsensor.lexTemperature(way1t)
	neg = string.sub(lgateway.lgetFieldValue(pac_kv, "way1t"), 1, 2)
	if neg == "00" then
		way1t = 0-way1t
	end
	local way1h = lgateway.lresoSLPacket(pac_kv, "way1h", 1, 4)
	way1h = lsensor.lexHumidity(way1h, A[1][1], Z[1][1])

	local way2t = lgateway.lresoSLPacket(pac_kv, "way2t", 3, 4)
	way2t = lsensor.lexTemperature(way2t)
	neg = string.sub(lgateway.lgetFieldValue(pac_kv, "way2t"), 1, 2)
	if neg == "00" then
		way2t = 0-way2t
	end
	local way2h = lgateway.lresoSLPacket(pac_kv, "way2h", 1, 4)
	way2h = lsensor.lexHumidity(way2h, A[1][2], Z[1][2])

	local way3t = lgateway.lresoSLPacket(pac_kv, "way3t", 3, 4)
	way3t = lsensor.lexTemperature(way3t)
	neg = string.sub(lgateway.lgetFieldValue(pac_kv, "way3t"), 1, 2)
	if neg == "00" then
		way3t = 0-way3t
	end
	local way3h = lgateway.lresoSLPacket(pac_kv, "way3h", 1, 4)
	way3h = lsensor.lexHumidity(way3h, A[1][3], Z[1][3])

	local data = string.format("SensorNO=%d;3_a01001_30-Rtd=%.2f,3_a01001_30-Flag=N;3_a01002_30-Rtd=%.2f,3_a01002_30-Flag=N;3_a01001_15-Rtd=%.2f,3_a01001_15-Flag=N;3_a01002_15-Rtd=%.2f,3_a01002_15-Flag=N;3_a01001_05-Rtd=%.2f,3_a01001_05-Flag=N;3_a01002_05-Rtd=%.2f,3_a01002_05-Flag=N", 
		node, way1t, way1h, way2t, way2h, way3t, way3h)

	return data
end