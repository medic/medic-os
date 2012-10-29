all:
	mkdir -p '$(ROOT)' && \
	./scripts/build-all '$(ROOT)' '$(START)' '$(START_VERSION)' \
        '$(STOP)' '$(STOP_VERSION)' '$(BITS)' '$(PARALLEL)'

clean-logs:
	rm -f logs/*.log

clean: clean-logs
	./scripts/clean-all; \
    (cd icu/source && make clean); \
    (cd js/src && make distclean); \
    (cd js/src && make -f Makefile.ref clean); \
    find . \( \( -name '*.o' -o -name '*.a' \) -a -type f \) -exec rm -f {} \;

stat:
	for file in logs/*"-$(TS).log"; do \
    wc -l < "$$file" \
      > "stats/`basename "$$file" .log | sed 's/-$(TS)//g;'`.count"; \
  done

