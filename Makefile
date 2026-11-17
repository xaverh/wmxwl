# wmxwl — POSIX make. Needs pkg-config and an installed neuswc.
# The != shell assignment is POSIX.1-2024 (GNU make 4, bmake).

CC ?= c99
PREFIX ?= /usr/local
PKG_CONFIG ?= pkg-config

PC_CFLAGS != $(PKG_CONFIG) --cflags swc wayland-server xkbcommon
PC_LIBS != $(PKG_CONFIG) --libs swc wayland-server xkbcommon libinput pixman-1 libdrm wld libudev

CFLAGS = $(PC_CFLAGS) -Isource -Isource/include -D_POSIX_C_SOURCE=200809L -O2 -Wall
LIBS = $(PC_LIBS)

OBJS = source/wmxwl.o source/util.o

.PHONY: all install clean

all: wmxwl

wmxwl: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

source/wmxwl.o: source/wmxwl.c source/config.h source/include/wmxwl.h source/include/types.h source/include/util.h
	$(CC) $(CFLAGS) -c -o $@ source/wmxwl.c

source/util.o: source/util.c source/include/util.h source/include/wmxwl.h source/include/types.h
	$(CC) $(CFLAGS) -c -o $@ source/util.c

install: wmxwl
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp wmxwl $(DESTDIR)$(PREFIX)/bin/wmxwl
	mkdir -p $(DESTDIR)$(PREFIX)/share/wayland-sessions
	cp wmxwl.desktop $(DESTDIR)$(PREFIX)/share/wayland-sessions/wmxwl.desktop

clean:
	rm -f $(OBJS) wmxwl
