local ip_buffer = lgateway.lnewBuffer()
lgateway.lrecvPacFromSocket(nil, "7770", 0, ip_buffer) --0:tcp, 1:udp
while 1 do
	lgateway.lrecvPacFromIPBuf(1, ip_buffer) -- 0:ASCII, 1:RTU
end
