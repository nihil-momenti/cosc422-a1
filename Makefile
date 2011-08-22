CC = g++

ORIGINAL_FLAGS := $(CPPFLAGS)
CPPFLAGS = -Wall -pedantic
CPPFLAGS += -g
#CPPFLAGS += -O3
CPPFLAGS += -MD -MP -MF .dep/$(subst /,-,$@).d
CPPFLAGS += $(shell pkg-config --cflags gl glu)
CPPFLAGS += $(ORIGINAL_FLAGS)

LDFLAGS += $(shell pkg-config --libs gl glu)


MISSING_PKG_CONFIG = WARNING:: \
Your system is missing the pkg-config info for glut. \
This is the case on all the COSC department linuxmachines \
so I will now attempt to use default build flags for it

CANT_FIND_GLUT_H = I couldnt find GL/glut.h so Im giving up

CANT_FIND_GLUT_SO = I couldnt find libglut.so so Im giving up

FOUND_GLUT = I found GL/glut.h and libglut.so so Im attempting to use the default flags \
  (CPPFLAGS += -I/usr/include) \
  (LDFLAGS += -lglut)

# Hackery cause the cosc linux installs are missing the glut pkg-config file.
ifeq "$(strip $(shell pkg-config --cflags glut))" ""
  $(warning $(MISSING_PKG_CONFIG))
  ifeq "$(strip $(shell locate 'GL/glut.h'))" ""
    $(error $(CANT_FIND_GLUT_H))
  else ifeq "$(strip $(shell locate '*/libglut.so'))" ""
    $(error $(CANT_FIND_GLUT_SO))
  else
    $(warning $(FOUND_GLUT))
    CPPFLAGS += -I/usr/include
    LDFLAGS += -lglut
  endif
else
  CPPFLAGS += $(shell pkg-config --cflags glut)
  LDFLAGS  += $(shell pkg-config --libs   glut)
endif

###############################################################################

C_FILES = $(wildcard src/*.cpp)
OBJECTS = $(C_FILES:.cpp=.o)

###############################################################################

all: build .dep basic
rebuild: clean all


basic: $(OBJECTS:src/%=build/%)
	$(CC) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)


build/%.o: src/%.cpp
	$(CC) $(CPPFLAGS) -o $@ -c $<

clean:
	rm -rf build/*
	rm -rf .dep/*


build:
	mkdir $@


.PRECIOUS: build/%.o
.PHONY: all rebuild clean

# Include the dependency files, should be the last of the makefile
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
