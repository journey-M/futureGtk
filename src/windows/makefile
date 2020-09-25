srcs := $(wildcard *.c)
objects := $(patsubst %.c,%.o, $(srcs))
destObjs := $(foreach n, $(objects), $(OBJDIR)/$(n))

mFlags = $(shell pkg-config --cflags glib-2.0 gtk+-3.0) 

$(OBJDIR)/%.o:%.c 
	gcc $(mFlags) -c $< -o $@ 

all: $(destObjs)
	@echo 编译完成： $(destObjs)
