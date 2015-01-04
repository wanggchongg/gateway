------------------------------------------------------------------------------------------------
-- the area of system's settings, begin
------------------------------------------------------------------------------------------------
data_type = 
{
	--字符串型， 无符号整型， 有符号整型， 变长代码型， 不确定
	C_String=1, U_Integer=2, S_Integer=3, V_Code=4, DIStr2DINum=5, HCStr2DINum=6, HCStr2DFStr=7, UCETN=-1,
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
	{"nodetype", 4, "UCETN", data_type.DIStr2DINum, 0},
	{"voltage", 4, "UCETN", data_type.HCStr2DINum, 0},
	{"light", 4, "UCETN", data_type.HCStr2DINum, 0},
	{"tempera", 4, "UCETN", data_type.HCStr2DINum, 0},
	{"humidity", 4, "UCETN", data_type.HCStr2DINum, 0},
	{"remain", 30, "UCETN", data_type.C_String, 0},
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
function resoSLPacket(pac_kv, ...)
	local node = lgateway.lresoSLPacket(pac_kv, "nodetype", 1, 4)

	local vol = lgateway.lresoSLPacket(pac_kv, "voltage", 1, 4)
	voltage = lsensor.lexVoltage(vol)

	local light = lgateway.lresoSLPacket(pac_kv, "light", 1, 4)
	light = lsensor.lexLight(vol, light)

	local tempera = lgateway.lresoSLPacket(pac_kv, "tempera", 1, 4)
	tempera = lsensor.lexOldTemperature(tempera)

	local humidity = lgateway.lresoSLPacket(pac_kv, "humidity", 1, 4)
	humidity = lsensor.lexOldHumidity(humidity, tempera)

	local data = string.format("SensorNO=%d;1_voltage-Rtd=%.2f,1_voltage-Flag=N;1_light-Rtd=%.2f,1_light-Flag=N;1_a01001-Rtd=%.2f,1_a01001-Flag=N;1_a01002-Rtd=%.2f,1_a01002-Flag=N", 
		node, voltage, light, tempera, humidity)

	return data
end