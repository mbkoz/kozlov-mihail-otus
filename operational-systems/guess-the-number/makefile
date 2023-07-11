SOURCES := $(wildcard *.c)
TARGETS := $(basename $(SOURCES))

.PHONY: all
all: $(TARGETS)

$(TARGETS): %: %.c
	gcc $^ -o $@

.PHONY: clean
clean:
	-rm $(TARGETS) 2> /dev/null