projDir := $(PWD)
buildDir := $(projDir)/build
objDir := $(buildDir)/objects
target := future

LDFLAGS +=-lm -lcurl 
LDFLAGS += $(shell pkg-config --libs glib-2.0 gtk+-3.0)


export CC := gcc
export FLAGS := -g
export BUILDDIR := $(buildDir)
export OBJDIR := $(objDir)

all: 
	$(CC) -o $(target) $(wildcard build/objects/*.o) $(LDFLAGS)

build:
	cd src && $(MAKE)



.PHONY: clean info build
info:
	@echo $(OBJDIR)
clean: 
	rm build/objects/*.o
