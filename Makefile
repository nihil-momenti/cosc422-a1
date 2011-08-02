CC = g++

ORIGINAL_FLAGS := $(CPPFLAGS)
CPPFLAGS = -Wall -pedantic
CPPFLAGS += -O3
CPPFLAGS += -MD -MP -MF .dep/$(subst /,-,$@).d
CPPFLAGS += $(shell pkg-config --cflags gl glu glut)
CPPFLAGS += $(ORIGINAL_FLAGS)

LDFLAGS += $(shell pkg-config --libs gl glu glut)

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
