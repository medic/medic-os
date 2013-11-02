
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
	for file in 'logs/${PLATFORM}'/*'-${TS}.log'; do \
		stats_file="stats/${PLATFORM}/` \
			basename "$$file" .log | sed 's/-${TS}//g;' \
		`.count" && \
		wc -l < "$$file" > "$$stats_file"; \
	done

