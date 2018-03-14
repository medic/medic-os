
# Variables

SHELL := /bin/bash


# Public targets

all: status
	@mkdir -p '${ROOT}' && \
	./scripts/build-all '${ROOT}' '${COMPILER_ROOT}' '${MEDIC_CORE_ROOT}'

clean: clean-logs
	./scripts/clean-all

stats: recalculate-stats

recalculate-stats:
	@for file in logs/*'-${TS}.log'; do \
	  stats_file="stats/` \
	    basename "$$file" .log | sed 's/-${TS}//g;' \
	  `.count" && \
	  wc -l < "$$file" > "$$stats_file"; \
	done


# Private targets

status:
	@echo && ./scripts/print-status 'start' \
        'Building package `medic-couch2pg`'

clean-logs:
	rm -f logs/*/*.log

