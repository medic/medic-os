
all:
	@mkdir -p '$(ROOT)' && \
	./scripts/build-all '$(ROOT)' '$(START)' '$(START_VERSION)' \
		'$(STOP)' '$(STOP_VERSION)' '$(BITS)' '$(PARALLEL)'

gardenize:
	./scripts/gardenize-couchdb '$(ROOT)'

clean-logs:
	rm -f logs/*.log

clean: clean-logs
	./scripts/clean-all; \
	find . \( \( -name '*.o' -o -name '*.a' \) -a -type f \) -exec rm -f {} \;

stats: recalculate-stats

recalculate-stats:
	for file in logs/*"-$(TS).log"; do \
		wc -l < "$$file" \
			> "stats/`basename "$$file" .log | sed 's/-$(TS)//g;'`.count"; \
	done

