LIBSRC = device.c usb.c
LIBOBJ = $(LIBSRC:.c=.o)

CPFLAGS = $(WIN32) -g -std=gnu99 -Wimplicit-function-declaration -I../include/tprog/

all: libtprog

include $(LIBSRC:.c=.d)

%.d: %.c
	$(CC) -M $(CPFLAGS) $(INCDIR) $< | sed 's/$*.o/& $@/g' >$@

%.o : %.c
	$(CC) -c $(CPFLAGS) $(INCDIR) $< -o $@

libtprog: $(LIBOBJ)
	ar r libtprog.a $(LIBOBJ)

install: libtprog
	cp libtprog.a ../lib

indent:
	for X in *.[ch]; do \
		indent -npsl -sob -bad -bli0 -cli2 $$X; \
		done

clean:
	rm -f *.o *.d *~
	rm -f libtprog.so
	rm -f libtprog.a
