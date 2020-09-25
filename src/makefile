#OBJDIR := ../build/objects

srcs := $(wildcard *.c)
objects := $(patsubst %.c,%.o, $(srcs))
destObjs := $(foreach n, $(objects), $(OBJDIR)/$(n))

subdir := requester jsonutil windows
#$(destObjs)
#$(subdir)
all:$(destObjs) $(subdir)
	@echo	编译结束 $(destObjs) 

$(subdir):
	cd $@ && $(MAKE)

$(OBJDIR)/%.o:%.c 
	gcc -c  $< -o $@ 

.PHONY:clean all $(subdir) 
