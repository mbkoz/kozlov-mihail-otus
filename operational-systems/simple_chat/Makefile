BUILD := debug

SRVDIR = chat_server_build
CLIDIR = chat_client_build
DBGDIR = debug
RELDIR = release

SRV_TARGET := chat_server
CLI_TARGET := char_client

CC = gcc
CFLAGS = -I . -I ./libso -std=gnu11
GDBFLAGS = -g
RELFLAGS = -O2
SRVFLAGS = -L ./libso -l ulist

FLAGS.debug := $(CFLAGS) $(GDBFLAGS)
FLAGS.release := $(CFLAGS) $(RELFLAGS)
FLAGS := $(FLAGS.$(BUILD))


SRV_SOURCES = sc_main.c user.c utils.c
SRV_OBJECTS = $(addsuffix .o, $(basename $(SRV_SOURCES)))

SRV_TARGETDIR.debug := $(SRVDIR)/$(DBGDIR)
SRV_TARGETDIR.release := $(SRVDIR)/$(RELDIR)
SRV_TARGETDIR := $(SRV_TARGETDIR.$(BUILD))

#.PHONY: all
#all: $(TARGETDIR) $(TARGETDIR)/$(TARGET)

.PHONY: $(SRV_TARGET)
$(SRV_TARGET): srv_dep $(SRV_TARGETDIR) $(SRV_TARGETDIR)/$(SRV_TARGET)

$(SRV_TARGETDIR):
	mkdir -p $(SRV_TARGETDIR)

$(SRV_TARGETDIR)/$(SRV_TARGET): $(addprefix $(SRV_TARGETDIR)/, $(SRV_OBJECTS))
	$(CC) $^ $(SRVFLAGS) -o $@

$(SRV_TARGETDIR)/%.o: %.c
	$(CC) $(FLAGS) -c $? -o $@

.PHONY: srv_dep
srv_dep:
	cd ulist_src && $(MAKE) lib
#	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./libso

.PHONY: clean
clean:
	-rm -rf $(SRVDIR) libso 2> /dev/null
