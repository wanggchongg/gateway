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
-- the area of GB_packet's format, start
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
-- the area of GB_packet's format, end
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
-- the area of user's function, start
-------------------------------------------------------------------------------------------------

--------------------------------------------------
-- global variable of user's function
-- 状态：
-- 0：无操作; 1：只返回应答; 2：返回应答后,返回执行结果;
-- 3：返回应答后.返回数据,返回执行结果; 5：返回应答后,连续返回数据
nxt_state = 0   --状态
END = 0
nCount = 1
SERV_PORT = nil -- PC's port
SERV_ADDR = nil -- PC's IP
--------------------------------------------------

--------------------------------------------------
-- [resoGBPacket ]
-- @return [0, success]
--------------------------------------------------
function resoGBPacket(...)
	nxt_state = 1
	GB_Flag_Event(lgateway.lgetFieldValue(s_pGBPac, "Flag"))
	while nxt_state ~= 0 do
		GB_QN_Event()
		GB_ST_Event(lgateway.lgetFieldValue(s_pGBPac, "ST"))
		GB_PW_Event()
		GB_CN_Event(lgateway.lgetFieldValue(s_pGBPac, "CN"))
		local pac_key1 =
		{
			"ST", "CN", "PW", "MN", "CP",
		}
		print("nCount:", nCount)
		if nCount <= 20 then
			nCount = nCount + 1
		else
			nxt_state = 0
		end
		judgePacketRT(pac_key1)
	end
	return 0
end

--------------------------------------------------
-- [judgePacketRT ]
-- @return [0, success]
--------------------------------------------------
function judgePacketRT(pac_key, ...)
	if END == 1 then
		local packet = lgateway.lconnectFields(a_pGBPac, pac_key, 0, 1)
		local dt_len = lgateway.lcountFieldsLen(packet, 4)
		lgateway.lassemFieldValue(a_pGBPac, "data_field_len", dt_len, 1)
		lgateway.lconnectField(a_pGBPac, "data_field_len", 0, 0)
		print("sended GB_packet:", lgateway.lassemPacket(a_pGBPac, check_attribute))
		lgateway.lsendGBPacket(SERV_ADDR, SERV_PORT, 0, 0)
		lgateway.lsleep(4)
	end
	return 0
end

--------------------------------------------------
-- [GB_Flag_Event ]
-- @return [0, success]
--------------------------------------------------
function GB_Flag_Event(value, ...)
	local cur_state = nxt_state
	if value == "0" then
		if cur_state == 1 then
			nxt_state = 2; return 0
		else
			return 0
		end
	elseif value == "1" then
		if cur_state == 1 then
			GB_Flag1_Action(); return 0
		else
			return 0
		end
	else
		nxt_state = 0; return 0
	end
	return 0
end

--------------------------------------------------
-- [GB_Flag1_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_Flag1_Action(...)
	GB_QN_Event()
	lgateway.lassemFieldValue(a_pGBPac, "Flag", "0", 1)
	GB_ST_Event(lgateway.lgetFieldValue(s_pGBPac, "ST"))
	GB_PW_Event()
	GB_CN_Event(lgateway.lgetFieldValue(s_pGBPac, "CN"))
	local pac_key2 =
	{
		"ST", "CN", "PW", "MN", "Flag", "CP",
	}
	judgePacketRT(pac_key2)
	return 0
end

--------------------------------------------------
-- [GB_QN_Event ]
-- @return [0, success]
--------------------------------------------------
function GB_QN_Event(...)
	local cur_state = nxt_state
	if cur_state == 1 then
		GB_QN_Action(); return 0
	elseif cur_state == 2 then
		GB_QN_Action(); return 0
	elseif cur_state == 3 then
		GB_QN_Action(); return 0
	else
		return 0
	end
	return 0;
end

--------------------------------------------------
-- [GB_QN_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_QN_Action(...)
	END = 0
	lgateway.lcopyFields(s_pGBPac, a_pGBPac, 1)
	lgateway.lassemFieldValue(a_pGBPac, "CP", "&&", 1)
	--print(lgateway.lgetFieldValue(a_pGBPac, "CP"))
	if nxt_state == 1 or nxt_state == 3 then
		local field = lgateway.lgetField(s_pGBPac, "QN")
		lgateway.lassemFieldValue(a_pGBPac, "CP", field, 0)
	end
	return 0
end

--------------------------------------------------
-- [GB_ST_Event ]
-- @param  value []
-- @return       [0, success]
--------------------------------------------------
function GB_ST_Event(value, ...)
	local cur_state = nxt_state
	if value == "32" then
		if cur_state == 1 then
			GB_ST91_Action(); return 0
		elseif cur_state == 2 then
			GB_ST32_Action(); return 0
		elseif cur_state == 3 then
			GB_ST91_Action(); return 0
		else
			return 0
		end
	elseif value == "91" then
		if cur_state == 1 then
			GB_ST91_Action(); return 0
		elseif cur_state == 3 then
			GB_ST91_Action(); return 0
		else
			return 0
		end
	else
		nxt_state = 0; return 0
	end
	return 0
end

--------------------------------------------------
-- [GB_ST32_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_ST32_Action(...)
	lgateway.lassemFieldValue(a_pGBPac, "ST", "32", 1)
	return 0
end

--------------------------------------------------
-- [GB_ST91_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_ST91_Action(...)
	lgateway.lassemFieldValue(a_pGBPac, "ST", "91", 1)
	return 0
end

--------------------------------------------------
-- [GB_CN_Event ]
-- @param  value []
-- @return       [0, success]
--------------------------------------------------
function GB_CN_Event(value, ...)
	local cur_state = nxt_state
	if value == "1011" then
		if cur_state == 1 then
			GB_CN9011_Action(); nxt_state = 2; return 0
		elseif cur_state == 2 then
			GB_CN1011_Action(); nxt_state = 3; return 0
		elseif cur_state == 3 then
			GB_CN9012_Action(); nxt_state = 0; return 0
		else
			return 0
		end
	elseif value == "1012" then
		if cur_state == 1 then
			GB_CN9011_Action(); nxt_state = 2; return 0
		elseif cur_state == 2 then
			GB_CN1012_Action(); nxt_state = 3; return 0
		elseif cur_state ==3 then
			GB_CN9012_Action(); nxt_state = 0; return 0
		else
			return 0
		end
	elseif value == "1072" then
		if cur_state == 1 then
			GB_CN9011_Action(); nxt_state = 2; return 0
		elseif cur_state == 2 then
			GB_CN1072_Action(); nxt_state = 3; return 0
		elseif cur_state == 3 then
			GB_CN9012_Action(); nxt_state = 0; return 0
		else
			return 0
		end
	elseif value == "2011" then
		if cur_state == 1 then
			GB_CN9011_Action(); nxt_state = 2; return 0
		elseif cur_state == 2 then
			GB_CN2011_Action(); nxt_state = 2; return 0
		else
			return 0
		end
	elseif value == "2012" then
		if cur_state == 1 then
			GB_CN9013_Action(); nxt_state = 0; return 0
		else
			return 0
		end
	elseif value == "2021" then
		if cur_state == 1 then
			GB_CN9011_Action(); nxt_state = 2; return 0
		elseif cur_state == 2 then
			GB_CN2021_Action(); nxt_state = 2; return 0
		else
			return 0
		end
	elseif value == "2022" then
		if cur_state == 1 then
			GB_CN9013_Action(); nxt_state = 0; return 0
		else
			return 0
		end
	elseif value == "2051" then
		if cur_state == 1 then
			GB_CN9011_Action(); nxt_state = 2; return 0
		elseif cur_state == 2 then
			GB_CN2051_Action(); nxt_state = 3; return 0
		elseif cur_state == 3 then
			GB_CN9012_Action(); nxt_state = 0; return 0
		else
			return 0
		end
	elseif value == "9021" then
		if cur_state == 1 then
			GB_CN9021_Action(); nxt_state = 0; return 0
		else
			return 0
		end
	else
		nxt_state = 0; return 0
	end
	return 0
end

--------------------------------------------------
-- [GB_CN1011_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_CN1011_Action(...)
	lgateway.lassemFieldValue(a_pGBPac, "CN", "1011", 1)
	local system_time = lgateway.lgetSystemTime(14)
	system_time = string.format("SystemTime=%s;", system_time)
	lgateway.lassemFieldValue(a_pGBPac, "CP", system_time, 0)
	GB_End_Action()
	return 0
end

--------------------------------------------------
-- [GB_CN1012_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_CN1012_Action(...)
	local CP_k, CP_v = lgateway.lsepField(s_pGBPac, "CP", 1)
	if CP_k == "SystemTime" then
		lgateway.lsetSystemTime(CP_v)
	end
	return 0
end

--------------------------------------------------
-- [GB_CN1072_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_CN1072_Action(...)
	local CP_k, CP_v = lgateway.lsepField(s_pGBPac, "CP", 1)
	print("CP_k:", CP_k, "CP_v:", CP_v)
	if CP_k == "PW" then
		local MN = lgateway.lgetFieldValue(s_pGBPac, "MN")
		lgateway.lsetPassWord(MN, CP_v)
	end
	return 0
end

--------------------------------------------------
-- [GB_CN2011_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_CN2011_Action(...)
	lgateway.lassemFieldValue(a_pGBPac, "CN", "2011", 1)
	local data_time = lgateway.lgetSystemTime(14)
	local MN = lgateway.lgetFieldValue(s_pGBPac, "MN")
	local Rtd = mainSLCtrol()
	Rtd = string.format("DataTime=%s;%s", data_time, Rtd)
	lgateway.lassemFieldValue(a_pGBPac, "CP", Rtd, 0)
	GB_End_Action()
	return 0
end

--------------------------------------------------
-- [mainSLCtrol ]
-- @return [0, success]
--------------------------------------------------
function mainSLCtrol(...)
	local SL_format = "./SL_format.lua"
	loadfile(SL_format)
	local s_pSLPac = lgateway.lnewPKV()
	lgateway.ldefPacketFormat(s_pSLPac, SL_format)
	lgateway.lrecSLPacket(SL_format, "SL_command")
	lgateway.lcheckPacket(s_pSLPac, check_attribute)
	lgateway.lsepPacket(s_pSLPac)
	return resoSLPacket(s_pSLPac)
end


--------------------------------------------------
-- [GB_CN2012_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_CN2012_Action(...)

	return 0
end

--------------------------------------------------
-- [GB_CN2021_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_CN2021_Action(...)
	lgateway.lassemFieldValue(a_pGBPac, "CN", "2021", 1)
	local data_time = lgateway.lgetSystemTime(14)
	local MN = lgateway.lgetFieldValue(s_pGBPac, "MN")
	local Rts = getState(MN)
	Rts = string.format("DataTime=%s;%s", data_time, Rts)
	lgateway.lassemFieldValue(a_pGBPac, "CP", Rts, 0)
	GB_End_Action()
	return 0
end

--------------------------------------------------
-- [GB_CN2022_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_CN2022_Action(...)

	return 0
end

--------------------------------------------------
-- [GB_CN2051_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_CN2051_Action(...)

	GB_End_Action()
	return 0
end

--------------------------------------------------
-- [GB_CN9011_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_CN9011_Action(...)
	lgateway.lassemFieldValue(a_pGBPac, "CN", "9011", 1)
	lgateway.lassemFieldValue(a_pGBPac, "CP", "QnRtn=1;", 0)
	GB_End_Action()
	return 0
end

--------------------------------------------------
-- [GB_CN9012_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_CN9012_Action(...)
	lgateway.lassemFieldValue(a_pGBPac, "CN", "9012", 1, 0)
	lgateway.lassemFieldValue(a_pGBPac, "CP", "ExeRtn=1;", 0)
	GB_End_Action()
	return 0
end

--------------------------------------------------
-- [GB_CN9013_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_CN9013_Action(...)
	lgateway.lassemFieldValue(a_pGBPac, "CN", "9013", 1)
	GB_End_Action()
	return 0
end

--------------------------------------------------
-- [GB_CN9021_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_CN9021_Action(...)
	lgateway.lassemFieldValue(a_pGBPac, "CN", "9022", 1)
	lgateway.lassemFieldValue(a_pGBPac, "CP", "Logon=1;", 0)
	GB_End_Action()
	return 0
end

--------------------------------------------------
-- [GB_PW_Event ]
-- @return [0, success]
--------------------------------------------------
function GB_PW_Event(...)
	local cur_state = nxt_state
	if cur_state == 1 then
		GB_PW_Action(); return 0
	else
		return 0
	end
	return 0
end

--------------------------------------------------
-- [GB_PW_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_PW_Action(...)
	local MN = lgateway.lgetFieldValue(s_pGBPac, "MN")
	local PW = lgateway.lgetPassWord(MN)
	local PW1 = lgateway.lgetFieldValue(s_pGBPac, "PW")
	if PW ~= PW1 then
		nxt_state = 0
	end
	return 0
end

--------------------------------------------------
-- [GB_End_Action ]
-- @return [0, success]
--------------------------------------------------
function GB_End_Action(...)
	lgateway.lassemFieldValue(a_pGBPac, "CP", "&&", 0)
	END = 1
	return 0
end
-------------------------------------------------------------------------------------------------
-- the area of user's function, end
-------------------------------------------------------------------------------------------------