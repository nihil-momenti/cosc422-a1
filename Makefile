CC = g++

CPPFLAGS += -Wall -pedantic
CPPFLAGS += -O3
CPPFLAGS += -MD -MP -MF .dep/$(subst /,-,$@).d
CPPFLAGS += $(shell pkg-config --cflags gl glu glut)

LDFLAGS += $(shell pkg-config --libs gl glu glut)


all: build .dep basic


rebuild: clean all


.dep build:
	mkdir $@


basic: build/model.o build/main.o
	$(CC) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)


build/%.o: src/%.cpp
	$(CC) $(CPPFLAGS) -o $@ -c $<


clean:
	rm -rf build/*
	rm -rf .dep/*


.PRECIOUS: build/%.o
.PHONY: all rebuild clean
