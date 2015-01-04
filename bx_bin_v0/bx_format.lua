--------------------------------------------------
-- global variable of user's function
BY_ADDR = "10.103.240.246" -- Beiyou's IP
BY_PORT = "7777" -- Beiyou's port
XD_ADDR = "10.103.240.246" -- Xida's IP
XD_PORT = "7777" -- Xida's port
--------------------------------------------------


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
function assemGBPacket(s_pGBPac, OPTION, ...)
	local Rtd = mainSLCtrol(OPTION)
	if Rtd == nil then
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

	local datatime = lgateway.lgetSystemTime(14)
	local gatewayno = lgateway.lgetCommandArgs("GATEWAY_NO")
	local CP = string.format("&&DataTime=%s;GatewayNO=%s;%s&&", datatime, gatewayno, Rtd)
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
	lgateway.lsendGBPacket(BY_ADDR, BY_PORT, 1, 0) -- 0:TCP, 1:UDP; 0:GB, 1:ASCII, 2:RTU
end

local LOC_PAC_OPT = 0
function mainSLCtrol(OPTION, ...)
	if OPTION == "1" then
		--simuLossPacket(1)
		SL_packet = lgateway.lrecSLPacket(nil, nil)
		lgateway.lsendGBPacket(XD_ADDR, XD_PORT, 1, 2) -- 0:TCP, 1:UDP; 0:GB, 1:ASCII, 2:RTU
		SL_packet = string.sub(SL_packet, string.len(SL_packet)/2+1, -1)
		lgateway.lsetSLPacket(SL_packet)
	else
		--lgateway.lsleep(10)
		LOC_PAC_OPT = LOC_PAC_OPT + 1
		LOC_PAC_OPT = LOC_PAC_OPT % 4
		SL_packet = lgateway.lrecXDPacket(LOC_PAC_OPT)
		lgateway.lsendGBPacket(XD_ADDR, XD_PORT, 1, 2) -- 0:TCP, 1:UDP; 0:GB, 1:ASCII, 2:RTU
	end

	if string.len(SL_packet) ~= 92 then
		return nil
	end
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
	end
	local s_pSLPac = lgateway.lnewPKV()
	lgateway.ldefPacketFormat(s_pSLPac, SL_format)
	lgateway.lcheckPacket(s_pSLPac, check_attribute)
	lgateway.lsepPacket(s_pSLPac)
	return resoSLPacket(s_pSLPac)
end

function simuLossPacket(n, ...)
	local nCount = 0
	while nCount < n do
		lgateway.lrecSLPacket(nil, nil)
		nCount = nCount + 1
	end
end
-------------------------------------------------------------------------------------------------
-- the area of IP_packet's function, end
-------------------------------------------------------------------------------------------------
