# есть 3 таргета: дебаг, тест (на основе дебаг скрипта, который запустит тест и сверит выход с референсом) и сборка библиотеки (release + define)
# все обьектные файлы должны располагаться в папке _build. 

OBJDIR = _build
LIBDIR = ullib
DBGDIR = debug
RELDIR = release

TARBASENAME = ullib
TARGET = $(TARBASENAME)
TARGETDIR = $(OBJDIR)/$(DBGDIR)

CC = gcc
CFLAGS := -I . -std=gnu11
LIBFLAGS = -D LIB_BUILD
GDBFLAGS = -g
RELFLAGS = -O2 -fPIC

SOURCES = $(wildcard *.c)
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))

.PHONY: all
all: debug test lib

.PHONY: debug
debug: CFLAGS += $(GDBFLAGS)
debug: TARGET = $(TARBASENAME)_test
debug: TARGETDIR = $(OBJDIR)/$(DBGDIR)
debug: $(TARGETDIR) $(TARGETDIR)/$(TARGET)

#.PHONY: test

$(TARGETDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $? -o $@

$(TARGETDIR)/$(TARGET): $(addprefix $(TARGETDIR)/, $(OBJECTS))
	$(CC) $? -o $@

$(TARGETDIR):
	mkdir -p $(TARGETDIR)

.PHONY: clean
clean:
	-rm -rf $(OBJDIR) 2> /dev/null

