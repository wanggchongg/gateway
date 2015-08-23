require("lgateway")
port =
{
	P_N = "/dev/ttyUSB0",  --串口路径,(Top6410: /dev/ttySAC0, EM9x60: /dev/ttyS5)
	B_R = 19200,           --波特率
	D_B = 8,               --数据位数
	P_R = 0,               --奇偶校验
	S_B = 1                --停止位数
}
lgateway.lsetPort(port.P_N, port.B_R, port.D_B, port.P_R, port.S_B)

GB_format = "./GB_format.lua"
loadfile(GB_format)

while 1 do
	s_pGBPac = lgateway.lnewPKV()
	a_pGBPac = lgateway.lnewPKV()

	lgateway.ldefPacketFormat(s_pGBPac, GB_format)

	lgateway.lrecGBPacket(nil, nil, 0, 0) -- arm board's IP and port
	lgateway.lcheckPacket(s_pGBPac, check_attribute) --transmit the start_code, check_code, end_code to lib
	lgateway.lsepPacket(s_pGBPac)
	lgateway.lcheckMonitorNum("77777770000001")
	lgateway.lsetPassWord("77777770000001", "123456")
	resoGBPacket()
end
