SOURCES = $(wildcard *.asm)
TARGETS = $(basename $(SOURCES))
GDBTRG = $(addsuffix _gdb, $(TARGETS))
OBJECTS = $(addsuffix .o, $(TARGETS))

INCPATH = -i ./3rd_party/
DEPS    = ./3rd_party/library.asm

NSMFLAGS  = $(INCPATH) -f elf -g -F dwarf
LDFLAGS  = -m elf_i386
NMFLAGS = -f sysv

.PHONY: all
all: $(TARGETS)

$(TARGETS): %: %.o
	ld $(LDFLAGS) -o $@ $^
	nm $(NMFLAGS) $^ $@ > sym_table

%.o: %.asm $(DEPS)
	nasm $(NSMFLAGS) -o $@ $<

.PHONY: $(GDBTRG)
$(GDBTRG): %_gdb: %
	gdb -x gdb_launch $^

.PHONY: clean
clean:
	-rm $(TARGETS) $(OBJECTS) sym_table 2> /dev/null
