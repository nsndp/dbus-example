# tutorial on makefiles: https://www3.ntu.edu.sg/home/ehchua/programming/cpp/gcc_make.html#zz-2.
# capturing dependencies: https://stackoverflow.com/a/15864028, https://stackoverflow.com/a/60827213

.PHONY: all clean

GIO = $(shell pkg-config --libs --cflags gio-2.0)
GIO := $(patsubst -I%, -isystem %, $(GIO))

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, obj/%.o, $(SRCS))
DEPS := $(patsubst %.c, obj/%.d, $(SRCS))

all: ../gserver

../gserver: $(OBJS)
	gcc -o $@ $^ $(GIO)

obj/%.o: %.c | obj
	gcc -o $@ $< $(GIO) -c -MMD -MP

-include $(DEPS)

obj:
	mkdir -p $@

clean:
	rm -rf obj ../gserver