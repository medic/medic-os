
all: tproglib sim turboadapter

tproglib: build-tproglib

build-tproglib:
	cd tproglib && $(MAKE)

sim: tproglib
	cd apps/sim/utils-src && $(MAKE)

turboadapter: tproglib
	cd apps/turboadapter/utils-src && $(MAKE)

clean: clean-tproglib clean-sim clean-turboadapter

clean-tproglib:
	cd tproglib && $(MAKE) clean

clean-sim:
	cd apps/sim/utils-src && $(MAKE) clean

clean-turboadapter:
	cd apps/turboadapter/utils-src && $(MAKE) clean

install: install-sim install-turboadapter

install-sim:
	for binary in cemu sim_dump; do \
		mkdir -p "$(PREFIX)/bin" && \
		cp "apps/sim/utils-src/$$binary" "$(PREFIX)/bin/"; \
	done

install-turboadapter:
	for binary in btldr dbg; do \
		mkdir -p "$(PREFIX)/bin" && \
		cp "apps/turboadapter/utils-src/$$binary" "$(PREFIX)/bin/"; \
	done

