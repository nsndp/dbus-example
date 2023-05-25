.PHONY: all clean

all:
	gcc -o ../gclient gclient.c $(shell pkg-config --libs --cflags gio-2.0)

clean:
	rm -f ../gclient