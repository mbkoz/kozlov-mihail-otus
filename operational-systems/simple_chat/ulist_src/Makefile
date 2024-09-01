# есть 2 таргета: 
# > либа (make lib) соберет release версию библиотеки и скопирует ее на уровень проекта чата
# > тест (make test) соберет debug версию, запустит проверку (скрипт в самом исходнике) и сравнит с эталоном

BUILD := debug

OBJDIR = _build
LIBDIR = libso
DBGDIR = debug
RELDIR = release

TARBASENAME := ulist

CC = gcc
CFLAGS = -I . -std=gnu11
GDBFLAGS = -g
RELFLAGS = -D LIB_BUILD -O2 -fPIC

SOURCES = $(wildcard *.c)
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))

TARGETDIR.debug := $(OBJDIR)/$(DBGDIR)
TARGETDIR.release := $(OBJDIR)/$(RELDIR)
TARGETDIR := $(TARGETDIR.$(BUILD))

TARGET.debug := $(TARBASENAME)_test
TARGET.release := lib$(TARBASENAME).so
TARGET := $(TARGET.$(BUILD))

FLAGS.debug := $(CFLAGS) $(GDBFLAGS)
FLAGS.release := $(CFLAGS) $(RELFLAGS)
FLAGS := $(FLAGS.$(BUILD))

LDFLAGS.release := -shared
LDFLAGS := $(LDFLAGS.$(BUILD))

.PHONY: all
all: $(TARGETDIR) $(TARGETDIR)/$(TARGET)

$(TARGETDIR):
	mkdir -p $(TARGETDIR)

$(TARGETDIR)/$(TARGET): $(addprefix $(TARGETDIR)/, $(OBJECTS))
	$(CC) $(LDFLAGS) $? -o $@


$(TARGETDIR)/%.o: %.c
	$(CC) $(FLAGS) -c $? -o $@

.PHONY: clean
clean:
	-rm -rf $(OBJDIR) 2> /dev/null


.PHONY: lib
lib:
	$(MAKE) BUILD=release
	mkdir -p ../$(LIBDIR)
	cp $(TARGETDIR.release)/$(TARGET.release) ../$(LIBDIR)/$(TARGET.release)
	cp ulist.h ../$(LIBDIR)/ulist.h

.PHONY: test
test:
	$(MAKE) BUILD=debug
	touch $(TARGETDIR.debug)/ul_file
	echo -n "" > $(TARGETDIR.debug)/ul_file
	touch $(TARGETDIR.debug)/out_file
	echo -n "" > $(TARGETDIR.debug)/out_file
	$(TARGETDIR.debug)/$(TARGET.debug) -f $(TARGETDIR.debug)/ul_file > $(TARGETDIR.debug)/out_file
	diff -s test_ref $(TARGETDIR.debug)/out_file

# кроме выпуска lib и debug надо добавить test deploy_lib для копирования библиотеки в место назначения

