
# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

INC = ../inc
LIB = ../lib

AR= ar rcu
RANLIB= ranlib
RM= -rm -f

# == END OF USER SETTINGS. NO NEED TO CHANGE ANYTHING BELOW THIS LINE =========

LUA_A=	$(LIB)/liblua.a
CORE_O=	lapi.o lcode.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o lmem.o \
	lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o ltm.o  \
	lundump.o lvm.o lzio.o
LIB_O=	lauxlib.o lbaselib.o ldblib.o liolib.o lmathlib.o loslib.o ltablib.o \
	lstrlib.o loadlib.o linit.o

ALL_O= $(CORE_O) $(LIB_O)

$(LUA_A): $(CORE_O) $(LIB_O)
	$(AR) $@ $(CORE_O) $(LIB_O)	# DLL needs all object files
	$(RANLIB) $@

clean:
	$(RM) $(LUA_A) $(ALL_O)

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: clean

# DO NOT DELETE

lapi.o: lapi.c
	$(CC) -c lapi.c -I$(INC)
lauxlib.o: lauxlib.c
	$(CC) -c lauxlib.c -I$(INC)
lbaselib.o: lbaselib.c
	$(CC) -c lbaselib.c -I$(INC)
lcode.o: lcode.c
	$(CC) -c lcode.c -I$(INC)
ldblib.o: ldblib.c
	$(CC) -c ldblib.c -I$(INC)
ldebug.o: ldebug.c
	$(CC) -c ldebug.c -I$(INC)
ldo.o: ldo.c
	$(CC) -c ldo.c -I$(INC)
ldump.o: ldump.c
	$(CC) -c ldump.c -I$(INC)
lfunc.o: lfunc.c
	$(CC) -c lfunc.c -I$(INC)
lgc.o: lgc.c
	$(CC) -c lgc.c -I$(INC)
linit.o: linit.c
	$(CC) -c linit.c -I$(INC)
liolib.o: liolib.c
	$(CC) -c liolib.c -I$(INC)
llex.o: llex.c
	$(CC) -c llex.c -I$(INC)
lmathlib.o: lmathlib.c
	$(CC) -c lmathlib.c -I$(INC)
lmem.o: lmem.c
	$(CC) -c lmem.c -I$(INC)
loadlib.o: loadlib.c
	$(CC) -c loadlib.c -I$(INC)
lobject.o: lobject.c
	$(CC) -c lobject.c -I$(INC)
lopcodes.o: lopcodes.c
	$(CC) -c lopcodes.c -I$(INC)
loslib.o: loslib.c
	$(CC) -c loslib.c -I$(INC)
lparser.o: lparser.c
	$(CC) -c lparser.c -I$(INC)
lstate.o: lstate.c
	$(CC) -c lstate.c -I$(INC)
lstring.o: lstring.c
	$(CC) -c lstring.c -I$(INC)
lstrlib.o: lstrlib.c
	$(CC) -c lstrlib.c -I$(INC)
ltable.o: ltable.c
	$(CC) -c ltable.c -I$(INC)
ltablib.o: ltablib.c
	$(CC) -c ltablib.c -I$(INC)
ltm.o: ltm.c
	$(CC) -c ltm.c -I$(INC)
lundump.o: lundump.c 
	$(CC) -c lundump.c -I$(INC)
lvm.o: lvm.c 
	$(CC) -c lvm.c -I$(INC)
lzio.o: lzio.c
	$(CC) -c lzio.c -I$(INC)
# (end of Makefile)
