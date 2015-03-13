export CC = gcc

INC = ./inc
LIB = ./lib

AR = ar -rcu
RANLIB = ranlib
RM = -rm -f
CFLAGS = -I$(INC) -L$(LIB) -llua -lgateway -ldl -lm -lpthread

GATEWAY_L = $(LIB)/libgateway.a
GATEWAY_O = lua_gateway.o gateway.o preprocess.o lua_auxi.o sensor.o

all: LUA_A lua

LUA_A:
	cd ./lua_src && $(MAKE)

lua: lua.o $(GATEWAY_L)
	$(CC) lua.o -olua $(CFLAGS)

$(GATEWAY_L): $(GATEWAY_O)
	$(AR) $@ $(GATEWAY_O)
	$(RANLIB) $@

lua.o: lua.c
	$(CC) -c lua.c -I$(INC)
lua_gateway.o: lua_gateway.c
	$(CC) -c lua_gateway.c -I$(INC)
gateway.o: gateway.c
	$(CC) -c gateway.c -I$(INC)
preprocess.o: preprocess.c
	$(CC) -c preprocess.c -I$(INC)
lua_auxi.o: lua_auxi.c
	$(CC) -c lua_auxi.c -I$(INC)
sensor.o: sensor.c
	$(CC) -c sensor.c -I$(INC)


.PHONY: clean LS
clean:
	$(RM) *.o lua $(GATEWAY_L) && \
	cd ./lua_src && $(MAKE) clean
LS:
	-ls /dev/ttyU*
