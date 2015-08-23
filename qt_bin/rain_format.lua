------------------------------------------------------------------------------------------------
-- the area of system's settings, begin
------------------------------------------------------------------------------------------------
data_type =
{
	--字符串型， 无符号整型， 有符号整型， 变长代码型， 不确定
	C_String=1, U_Integer=2, S_Integer=3, V_Code=4, DStr2DINum=5, HStr2DINum=6, HStr2DFStr=7, HStr2DU_INum = 8, UCETN=-1,
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
	{"address", 2, "UCETN", data_type.C_String, 0},
	{"extt", 4, "UCETN", data_type.HStr2DU_INum, 0},
	{"intt", 4, "UCETN", data_type.HStr2DU_INum, 0},
	{"ph", 4, "UCETN", data_type.HStr2DU_INum, 0},
	{"rainfall", 4, "UCETN", data_type.HStr2DU_INum, 0},
	{"wspeed", 4, "UCETN", data_type.HStr2DU_INum, 0},
	{"wdirec", 4, "UCETN", data_type.HStr2DU_INum, 0},
	{"voltage", 4, "UCETN", data_type.HStr2DU_INum, 0},
	{"rainyes", 2, "UCETN", data_type.HStr2DU_INum, 0},
	{"remain", 6, "UCETN", data_type.C_String, 0},
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
	if node ~= 2 then
		return nil
	end

	local extt_i = lgateway.lresoSLPacket(pac_kv, "extt", 1, 2)
	local extt_d = lgateway.lresoSLPacket(pac_kv, "extt", 3, 2)
	local extt = string.format("%d.%d", extt_i, extt_d)

	local intt_i = lgateway.lresoSLPacket(pac_kv, "intt", 1, 2)
	local intt_d = lgateway.lresoSLPacket(pac_kv, "intt", 3, 2)
	local intt = string.format("%d.%d", intt_i, intt_d)

	local ph_i = lgateway.lresoSLPacket(pac_kv, "ph", 1, 2)
	local ph_d = lgateway.lresoSLPacket(pac_kv, "ph", 3, 2)
	local ph = string.format("%d.%d", ph_i, ph_d)

	local rainfall_i = lgateway.lresoSLPacket(pac_kv, "rainfall", 1, 2)
	local rainfall_d = lgateway.lresoSLPacket(pac_kv, "rainfall", 3, 2)
	local rainfall = string.format("%d.%d", rainfall_i, rainfall_d)

	local wspeed_i = lgateway.lresoSLPacket(pac_kv, "wspeed", 1, 2)
	local wspeed_d = lgateway.lresoSLPacket(pac_kv, "wspeed", 3, 2)
	local wspeed = string.format("%d.%d", wspeed_i, wspeed_d)

	local wdirec_i = lgateway.lresoSLPacket(pac_kv, "wdirec", 1, 2)
	local wdirec_d = lgateway.lresoSLPacket(pac_kv, "wdirec", 3, 2)
	local wdirec = string.format("%d.%d", wdirec_i, wdirec_d)

	local voltage_i = lgateway.lresoSLPacket(pac_kv, "voltage", 1, 2)
	local voltage_d = lgateway.lresoSLPacket(pac_kv, "voltage", 3, 2)
	local voltage = string.format("%d.%d", voltage_i, voltage_d)

	local rainyes = lgateway.lresoSLPacket(pac_kv, "rainyes", 1, 2)

	local data = string.format("SensorNO=%d;2_a01001_e-Rtd=%s,2_a01001_e-Flag=N;2_a01001_i-Rtd=%s,2_a01001_i-Flag=N;2_a06003-Rtd=%s,2_a06003-Flag=N;2_a06001-Rtd=%s,2_a06001-Flag=N;2_a01007-Rtd=%s,2_a01007-Flag=N;2_a01008-Rtd=%s,2_a01008-Flag=N;2_voltage-Rtd=%s,2_voltage-Flag=N;2_a06002-Rtd=%d,2_a06002-Flag=N",
		node, extt, intt, ph, rainfall, wspeed, wdirec, voltage, rainyes)

	return data
end