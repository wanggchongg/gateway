dofile("./config.lua")

LOC_PAC_OPT = 0
function transmit(OPTION, GATEWAY_NO, SL_buffer, XD_buffer, BY_buffer, ...)
	if OPTION == "1" then
		SL_packet, SL_buflen = lgateway.lrecvPacFromSLBuf(SL_buffer)
		local SL_len = string.len(SL_packet)
		if SL_len ~= 184 then
			return nil
		end
		if XD_ADDR ~= nil and SL_buflen == 92 then
			lgateway.lsendPacToIPBuf(2, XD_buffer) -- 0:GB, 1:ASCII, 2:RTU
		end
		SL_packet = string.sub(SL_packet, string.len(SL_packet)/2+1, -1)
	else
		LOC_PAC_OPT = LOC_PAC_OPT + 1
		LOC_PAC_OPT = LOC_PAC_OPT % 4
		SL_packet = lgateway.lrecXDPacket(LOC_PAC_OPT)
		if XD_ADDR ~= nil then
			lgateway.lsendPacToIPBuf(2, XD_buffer) -- 0:GB, 1:ASCII, 2:RTU
		end
		lgateway.lsleep(INTERVAL)
	end

	local DATA_TIME = lgateway.lgetSystemTime(14)
	local GBRT_packet = string.format("%03d%s%s", GATEWAY_NO, DATA_TIME, SL_packet)
	if string.len(GBRT_packet) < 109 then
		return nil
	end
	print("bupt's pac:", GBRT_packet)
	lgateway.lsetGBRTPacket(GBRT_packet)
	if BY_ADDR ~= nil then
		lgateway.lsendPacToIPBuf(0, BY_buffer) -- 0:GB, 1:ASCII, 2:RTU
	end
end

function main(...)
	local OPTION = lgateway.lgetCommandArgs("OPTION")
	local GATEWAY_NO = lgateway.lgetCommandArgs("GATEWAY_NO")
	if string.len(GATEWAY_NO) > 3 or string.len(GATEWAY_NO) < 1 then
		error("set GATEWAY_NO error")
	end

	if OPTION == "1" then -- serial_Recv thread
		local SL_conf = {SL_port.PATH, SL_port.BAUD, SL_port.DATA, SL_port.PARI, SL_port.STOP}
		SL_buffer = lgateway.lnewBuffer()
		lgateway.lrecvPacFromSerial(nil, SL_buffer, SL_conf) --
	end

	if XD_ADDR ~= nil then -- tcp_Send thread
		XD_buffer = lgateway.lnewBuffer()
		lgateway.lsendPacToSocket(XD_ADDR, XD_PORT, 0, 1, XD_buffer) -- 0:TCP, 1:UDP;
	end

	if BY_ADDR ~= nil then -- udp_Send thread
		BY_buffer = lgateway.lnewBuffer()
		lgateway.lsendPacToSocket(BY_ADDR, BY_PORT, 1, 1, BY_buffer) -- 0:TCP, 1:UDP;
	end

	while true do
		transmit(OPTION, GATEWAY_NO, SL_buffer, XD_buffer, BY_buffer)
	end
end

main()