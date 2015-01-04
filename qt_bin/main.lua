------------------------------------------------------------------------------------------------
-- the area of system's settings, start
------------------------------------------------------------------------------------------------
data_type =
{
	--字符串型， 无符号整型， 有符号整型， 变长代码型， 不确定
	C_String=1, U_Integer=2, S_Integer=3, V_Code=4, DIStr2DINum=5, HCStr2DINum=6, HCStr2DFStr=7, UCETN=-1,
}

CRC =
{
	CRC16=1, CRC32=2, NONE=0,
}

check_attribute = --transmit the start_code,check_code,end_code's name to lib of gataway
{
	[1] = "start_code",
	[2] = "check_code",
	[3] = "end_code",
}
------------------------------------------------------------------------------------------------
-- the area of system's settings, end
------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------
-- the area of IP_packet's format, start
------------------------------------------------------------------------------------------------
command_type = 2 -- 1, only have attribute value; 2, have attribute key and value
space_mark_fd = ";"
space_mark_kv = "="
space_mark_lr = "&&"
check_type = CRC.CRC32

data_field =
{
----------{attribute key, 字段长度, 字段值, 数据类型, have key?}
	[1] = {"QN", 17, "UCETN", data_type.C_String, 1},
	[2] = {"ST", -1, "UCETN", data_type.C_String, 1},
	[3] = {"CN", -1, "UCETN", data_type.C_String, 1},
	[4] = {"PW", 6, "UCETN", data_type.C_String, 1},
	[5] = {"MN", 14, "UCETN", data_type.C_String, 1},
	[6] = {"Flag", -1, "UCETN", data_type.C_String, 1},
	[7] = {"CP", -1, "UCETN", data_type.V_Code, 1},
}

-- 0 无此字段, -1 此字段值不确定
packet =
{
----------{attribute key, 字段长度, 字段值, 数据类型, have key?}
	[1] = {"start_code", 2, "##", data_type.C_String, 0}, --开始符号
	[2] = {"data_field_len", 4, "UCETN", data_type.S_Integer, 0}, --数据段长度
	[3] = {"data_code", -1, "data_field", data_type.UCETN, 0}, --数据段
	[4] = {"check_code", 4, "UCETN", data_type.C_String, 0}, --校验段
	[5] = {"end_code", 2, "\r\n", data_type.C_String, 0}, --结束符
}
-------------------------------------------------------------------------------------------------
-- the area of IP_packet's format, end
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
-- the area of IP_packet's function, start
-------------------------------------------------------------------------------------------------
function mainIPCtrl(udpRecv_buf, ... )
	local IP_packet = lgateway.lrecvPacFromIPBuf(0, udpRecv_buf) -- 0:ASCII, 1:RTU
	if string.len(IP_packet) ~= 109 then
		return nil
	end
	local gatewayno = string.sub(IP_packet, 1, 3)
	local datatime = string.sub(IP_packet, 4, 17)
	local SL_packet = string.sub(IP_packet, 18, -1)
	lgateway.lsetSLPacket(SL_packet)
	local sensor = string.sub(SL_packet, 37, 40)
	-- print("sensor:", sensor)
	if sensor == "0001" then
		SL_format = "./old_format.lua"
	elseif sensor == "0002" then
		SL_format = "./rain_format.lua"
	elseif sensor == "0003" then
		SL_format = "./TH_format.lua"
	elseif sensor == "0004" then
		SL_format = "./salt_format.lua"
	elseif sensor == "0005" then
		SL_format = "./shock_format.lua"
	end
	local s_pSLPac = lgateway.lnewPKV()
	lgateway.ldefPacketFormat(s_pSLPac, SL_format)
	if lgateway.lcheckPacket(s_pSLPac, check_attribute) == false then
		return nil
	end
	lgateway.lsepPacket(s_pSLPac)
	return datatime, gatewayno, resoSLPacket(s_pSLPac)
end

function assemGBPacket(s_pGBPac, udpRecv_buf, udpSend_buf, ...)
	local dataTime, gatewayNo, Rtd = mainIPCtrl(udpRecv_buf)
	if dataTime == nil then
		return nil
	end
	local ST = "32"
	lgateway.lassemFieldValue(s_pGBPac, "ST", ST, 1)
	local CN = "2011"
	lgateway.lassemFieldValue(s_pGBPac, "CN", CN, 1)
	local PW = "12345"
	lgateway.lassemFieldValue(s_pGBPac, "PW", PW, 1)
	local MN = "88888800000001"
	lgateway.lassemFieldValue(s_pGBPac, "MN", MN, 1)

	local CP = string.format("&&DataTime=%s;GatewayNO=%d;%s&&", dataTime, gatewayNo, Rtd)
	lgateway.lassemFieldValue(s_pGBPac, "CP", CP, 1)

	local pac_key =
	{
		"ST", "CN", "PW", "MN",
	}
	lgateway.lconnectFields(s_pGBPac, pac_key, 0, 1)
	local packet = lgateway.lconnectField(s_pGBPac, "CP", 1, 1)
	local data_len = lgateway.lcountFieldsLen(packet, 4)
	lgateway.lassemFieldValue(s_pGBPac, "data_field_len", data_len, 1)
	lgateway.lconnectField(s_pGBPac, "data_field_len", 1, 0)
	print("sended GB_packet:", lgateway.lassemPacket(s_pGBPac, check_attribute))
	lgateway.lsendPacToIPBuf(0, udpSend_buf) -- 0:GB, 1:ASCII, 2:RTU
end


function main( ... )
	local s_pGBPac = lgateway.lnewPKV()
	lgateway.ldefPacketFormat(s_pGBPac, nil)

	local udpRecv_buf = lgateway.lnewBuffer() -- udp_Recv thread
	lgateway.lrecvPacFromSocket(nil, "7777", 1, udpRecv_buf) -- 0:TCP, 1:UDP;

	local udpSend_buf = lgateway.lnewBuffer() -- udp_Send thread
	lgateway.lsendPacToSocket("127.0.0.1", "7778", 1, udpSend_buf) -- 0:TCP, 1:UDP;

	while true do
		assemGBPacket(s_pGBPac, udpRecv_buf, udpSend_buf)
	end
end
-------------------------------------------------------------------------------------------------
-- the area of IP_packet's function, end
-------------------------------------------------------------------------------------------------
main()