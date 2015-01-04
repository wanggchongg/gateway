port = 
{
	P_N = "/dev/ttyUSB0",  --串口路径,(Top6410: /dev/ttySAC0)
	B_R = 57600,           --波特率 
	D_B = 8,               --数据位数
	P_R = 0,               --奇偶校验
	S_B = 1                --停止位数
}

local OPTION = lgateway.lgetCommandArgs("OPTION")
if OPTION == "1" then
	lgateway.lsetPort(port.P_N, port.B_R, port.D_B, port.P_R, port.S_B)
end

local s_pGBPac = lgateway.lnewPKV()
lgateway.ldefPacketFormat(s_pGBPac, "./bx_format.lua")

local nCount = 1
while 1 do
	print(nCount)
	nCount = nCount + 1
	assemGBPacket(s_pGBPac, OPTION)
	lgateway.lsleep(5)
end
