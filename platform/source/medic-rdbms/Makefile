
# Variables

PARALLEL ?= 16
SHELL := /bin/bash
PLATFORM := $(shell ../../scripts/detect-platform)


# Public targets

all: status
	@mkdir -p '${ROOT}' && \
	./scripts/build-all '${ROOT}' '${START}' '${START_VERSION}' \
	  '${STOP}' '${STOP_VERSION}' '${PLATFORM}' '${BITS}' '${PARALLEL}'

clean: clean-logs
	./scripts/clean-all

stats: recalculate-stats

recalculate-stats:
	@for file in 'logs/${PLATFORM}'/*'-${TS}.log'; do \
	  stats_file="stats/${PLATFORM}/` \
	    basename "$$file" .log | sed 's/-${TS}//g;' \
	  `.count" && \
	  wc -l < "$$file" > "$$stats_file"; \
	done


# Private targets

status:
	@echo && ./scripts/print-status 'start' \
        'Building package `medic-rdbms` for `${PLATFORM}`'

clean-logs:
	rm -f logs/*/*.log

