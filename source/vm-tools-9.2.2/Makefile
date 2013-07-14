
unexport PLATFORM

all:
	@mkdir -p '${ROOT}' && \
	./scripts/build-all '${ROOT}' '${START}' '${START_VERSION}' \
		'${STOP}' '${STOP_VERSION}' '${PLATFORM}' '${BITS}' \
		'${PARALLEL}' '${KERNEL_VERSION}' '${KERNEL_MODULE_PATH}'

clean-logs:
	rm -f logs/*.log

clean: clean-logs
	./scripts/clean-all

stats: recalculate-stats

recalculate-stats:
	for file in logs/*"-${TS}.log"; do \
		wc -l < "$$file" \
			> "stats/`basename "$$file" .log | sed 's/-${TS}//g;'`.count"; \
	done

