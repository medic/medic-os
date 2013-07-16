ifndef TPROG_DIR
TPROG_DIR = ../../..
endif

ifndef TURBO_DIR
TURBO_DIR := ../../../../turbo-devel-1.5.24
endif

INCDIR	= -I$(TPROG_DIR)/include/ -I.  -I$(TURBO_DIR)

RM = rm -f

SRC = btldr.c  dbg.c

LIB = -L$(TPROG_DIR)/tproglib -ltprog

CFLAGS	= $(WIN32) -std=gnu99 -Wimplicit-function-declaration
LDFLAGS = $(LIB)

OBJ = $(SRC:.c=.o)
BIN = $(SRC:.c=)
 
all: $(BIN)

include $(SRC:.c=.d)

%.o : %.c 
	$(CC) -c $(CFLAGS) $(INCDIR) $< -o $@

%.d: %.c
	$(CC) -M $(CFLAGS) $(INCDIR) $< | sed 's/$*.o/& $@/g' >$@

$(BIN): $(OBJ)
	$(CC) -o $@ $@.o $(LDFLAGS)

indent:
	for X in $(SRC); do \
		indent -npsl -sob -bad -bli0 -cli2 $$X; \
		done

install: all
	cp $(BIN) ../../../bin

clean:
	$(RM) *.o
	$(RM) *.d
	$(RM) *.exe
	$(RM) *~
	$(RM) $(BIN)
