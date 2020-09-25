srcs := $(wildcard *.c)
objects := $(patsubst %.c,%.o, $(srcs))
destObjs := $(foreach n, $(objects), $(OBJDIR)/$(n))

$(OBJDIR)/%.o:%.c 
	gcc -c  $< -o $@ 

all: $(destObjs)
