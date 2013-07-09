ifndef TPROG_DIR
TPROG_DIR = ../../../
endif

ifndef TURBO_DIR
TURBO_DIR := ../../../../../turbo-devel
endif

TURBO_TAG = #--turbo-manifest "Version: 2.0.0 Vendor: BLADOX"  #--turbo-verbose
INCDIR  = -I$(TURBO_DIR)/include -I$(TPROG_DIR)/include -I.
LIBDIR = $(TURBO_DIR)/lib

CC = avr-gcc
LD = avr-ld
RM = rm -f

TRG = sim
SRC = sim.c uart.c clka.c

LIB = 

CFLAGS	= -std=gnu99 -mmcu=atmega128 -mno-tablejump -Wimplicit-function-declaration -Os -fno-builtin
LDFLAGS = -L$(LIBDIR) -T $(TPROG_DIR)/lib/turbo.lds -d -r --emit-relocs -R $(TPROG_DIR)/lib/public_calls $(LIB) -lm -lc `avr-gcc -print-libgcc-file-name`

OBJ = $(SRC:.c=.o)
  
all:	$(TRG).trb

include $(SRC:.c=.d)

%.o : %.c 
	$(CC) -c $(CFLAGS) $(INCDIR) $< -o $@

%.d: %.c
	$(CC) -M $(CFLAGS) $(INCDIR) $< | sed 's/$*.o/& $@/g' >$@

$(TRG).elf: $(OBJ)
	$(LD) -o $@ $(OBJ) $(LDFLAGS)

$(TRG).trb: $(TRG).elf
	avr-objdump $(TURBO_TAG) --turbo $(TRG).elf

dis: $(TRG).elf
	avr-objdump -D --architecture=avr:5  $(TRG).elf >$(TRG).dis

indent:
	for X in $(SRC); do \
		indent -npsl -sob -bad -bli0 -cli2 $$X; \
		done

install: all
	cp $(TRG).trb ../../../modules

clean:
	$(RM) *.o
	$(RM) *.d
	$(RM) *~
	$(RM) $(TRG).dis
	$(RM) $(TRG).elf
	$(RM) $(TRG).trb
