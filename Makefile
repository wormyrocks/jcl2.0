OS=$(shell uname)
ifneq (,$(findstring MINGW,$(OS)))
EXTENSION=.exe

# These are important
LDIRS=
LIBS=-lpthread -lsetupapi -lIphlpapi
EXTRA_CFLAGS=-static -static-libgcc -static-libstdc++ -O4
PLATFORM=windows

endif
ifeq ($(OS),Linux)
endif
ifeq ($(OS),Darwin)
LDIRS=
LIBS=-framework IOKit -framework CoreFoundation
EXTENSION=
EXTRA_CFLAGS=
PLATFORM=mac
endif

CC=g++
TARGET=run
# These are important
INCLUDE=hidapi/hidapi
CLASSLIST=

ODIR=obj
ODIR2=cl
LDIR=$(foreach ld, $(LDIRS),-Llibs/$(ld))
ROOT_CFLAGS=-I. $(foreach incname, $(INCLUDE),-Ilibs/$(incname)) $(LDIR)
CFLAGS=$(ROOT_CFLAGS)
CPPFLAGS=$(ROOT_CFLAGS) $(EXTRA_CFLAGS) $(LIBS) -std=c++11
CLASSES=$(foreach classname, $(CLASSLIST),$(ODIR2)/$(classname).o)
OBJS=$(TARGET).o $(CLASSES)
OBJ=$(patsubst %,$(ODIR)/%,$(OBJS))

$(TARGET)$(EXTENSION): $(OBJ) $(ODIR)/Joycon.o $(ODIR)/hid.o
	$(CC) -g -o $@ $^ $(CPPFLAGS)

$(ODIR)/hid.o: libs/hidapi/$(PLATFORM)/hid.c
	gcc -c -g -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.cpp %.h helpers.h
	$(CC) -c -g -o $@ $< $(CPPFLAGS)

.PHONY: clean

clean:
	rm -f $(TARGET)$(EXTENSION); rm -f $(ODIR)/*.o; rm -f $(ODIR)/$(ODIR2)/*.o
