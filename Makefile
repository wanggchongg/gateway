CC = gcc

all: first_layer phclient

first_layer: second_layer
	cp lua bx_bin_v0 && cp lua bx_bin_v1 && cp lua ph_bin && cp lua qt_bin
second_layer:
	cd ./gw_src && $(MAKE) && \
	cp ./lua ../ 

phclient: phclient.c
	$(CC) $< -o $@


.PHONY: clean LS
clean:
	-rm -f phclient lua bx_bin_v0/lua bx_bin_v1/lua ph_bin/lua qt_bin/lua && \
	cd ./gw_src && make clean

LS:
	-ls /dev/ttyU*
