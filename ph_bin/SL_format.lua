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
	"start_code","check_code", "end_code",
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
check_type = CRC.CRC16
packet = 
{
	{"address", 2, "UCETN", data_type.C_String, 0},
	{"action", 2, "UCETN", data_type.C_String, 0},
	{"data_field_len", 2, "UCETN", data_type.S_Integer, 0},
	{"data_code", -1, "UCETN", data_type.HCStr2DFStr, 0},
	{"check_code", 4, "UCETN", data_type.C_String, 0},
}
-------------------------------------------------------------------------------------------------
-- the format of serial packet, end
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
-- the area of user's function, begin
-------------------------------------------------------------------------------------------------
--------------------------------------------------
-- [resoSLPacket ]
-- @return [0, success]
--------------------------------------------------
function resoSLPacket(pac_kv, ...)
	local data1 = lgateway.lresoSLPacket(pac_kv, "data_code", 1, 8)
	data1 = string.match(data1,"%d+.+%d")

	local data2 = lgateway.lresoSLPacket(pac_kv, "data_code", 9, 8)
	data2 = string.match(data2,"%d+.+%d")

	local data  = string.format("PH=%s,Temperature=%s",data1, data2)

	local data3 = os.date("%Y%m%d%H%M%S",os.time())
	data3  = string.format("%s,%s\n",data3, data)

	local fd = io.open("./SL_log", "a+")
	fd:write(data3)
	fd:close()

	return data
end
-------------------------------------------------------------------------------------------------
-- the area of user's function, end
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
-- the area of user's config, start
-------------------------------------------------------------------------------------------------

SL_command = { 0x01, 0x04, 0x9C, 0x40, 0x00, 0x04, 0xDE, 0x4D }
-------------------------------------------------------------------------------------------------
-- the area of user's config, end
-------------------------------------------------------------------------------------------------