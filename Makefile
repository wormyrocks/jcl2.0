$(shell mkdir -p obj)
OS=$(shell uname)
ifneq (,$(findstring MINGW,$(OS)))
EXTENSION=.exe
LIBEXTENSION=.dll

# These are important
LDIRS=
SWITCHES=-lpthread -lsetupapi -lIphlpapi
LIB_CPPFLAGS=-shared -Wl,--out-implib,obj/$(LIBTARGET).a
EXTRA_CFLAGS=-static-libstdc++ -O4
PLATFORM=windows
endif
ifeq ($(OS),Linux)
SWITCHES=-pthread -ludev
LIB_CPPFLAGS=-shared
EXTRA_CPPFLAGS=#-I/usr/include/libusb-1.0
PLATFORM=linux
LIBEXTENSION=.so
endif
ifeq ($(OS),Darwin)
SWITCHES=-framework IOKit -framework CoreFoundation
EXTENSION=
EXTRA_CFLAGS=
PLATFORM=mac
LIB_CPPFLAGS=-dynamiclib -fPIC
LIBEXTENSION=.dylib
endif

CC=g++
DEMOTARGET=jcdemo
LIBNAME=joycon
LIBTARGET=lib$(LIBNAME)
CLASSNAME=Joycon
INCLUDE=hidapi/hidapi
INCLUDES=-I. $(foreach incname, $(INCLUDE),-Ilibs/$(incname))$(LDIR)

CPPFLAGS=-std=c++11 $(DEBUGFLAGS)
DEBUGFLAGS=-g -DDEBUG

demo: lib $(DEMOTARGET)$(EXTENSION)
lib: $(LIBTARGET)$(LIBEXTENSION)
nodebug: demo

# Joycon.o
obj/$(CLASSNAME).o: $(CLASSNAME)/$(CLASSNAME).cpp $(CLASSNAME)/$(CLASSNAME).h
	$(CC) -c -o $@ $< $(CPPFLAGS) $(INCLUDES) -DBUILDING_SHARED_LIB

# libjoycon.dll/dylib
$(LIBTARGET)$(LIBEXTENSION): obj/$(CLASSNAME).o obj/$(LIBTARGET).o obj/hid.o
	$(CC) -o $@ $^ $(CPPFLAGS) $(SWITCHES) $(LIB_CPPFLAGS) $(EXTRA_CFLAGS) -DBUILDING_SHARED_LIB

# libjoycon.o
obj/$(LIBTARGET).o: $(LIBTARGET).cpp $(LIBTARGET).h
	$(CC) -c -o $@ $< $(CPPFLAGS) $(INCLUDES) -DBUILDING_SHARED_LIB

# hid.o
obj/hid.o: libs/hidapi/$(PLATFORM)/hid.c
	gcc -c -o $@ $< $(INCLUDES) -static-libgcc -static

# libdemo
$(DEMOTARGET)$(EXTENSION): obj/$(DEMOTARGET).o
	$(CC) -o $@ $^ $(CPPFLAGS) -L. -l$(LIBNAME)

# libdemo.o
obj/$(DEMOTARGET).o: $(DEMOTARGET).cpp
	$(CC) -c -o $@ $< $(CPPFLAGS)

.PHONY: clean

clean:
	rm -f $(LIBTARGET)$(LIBEXTENSION); rm -f $(DEMOTARGET)$(EXTENSION); rm -rf obj; mkdir -p obj