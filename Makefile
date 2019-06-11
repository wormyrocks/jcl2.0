OS=$(shell uname)
ifneq (,$(findstring MINGW,$(OS)))
EXTENSION=.exe

# These are important
LDIRS=
LIBS=-lsetupapi
EXTRA_CFLAGS=-static -static-libgcc -static-libstdc++
PLATFORM=windows

endif
ifeq ($(OS),Linux)
endif
ifeq ($(OS),Darwin)
LDIRS=
LIBS=-framework IOKit
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

$(TARGET)$(EXTENSION): $(OBJ) $(ODIR)/hid.o
	$(CC) -g -o $@ $^ $(CPPFLAGS)

$(ODIR)/$(ODIR2)/%.o: %/*.cpp %/*.h
	$(CC) -c -o $@ $< $(CPPFLAGS)

$(ODIR)/hid.o: libs/hidapi/$(PLATFORM)/hid.c
	gcc -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.cpp %.h
	$(CC) -c -o $@ $< $(CPPFLAGS)

.PHONY: clean

clean:
	rm -f $(TARGET)$(EXTENSION); rm -f $(ODIR)/*.o; rm -f $(ODIR)/$(ODIR2)/*.o
