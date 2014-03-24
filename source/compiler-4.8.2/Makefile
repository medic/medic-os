
PARALLEL ?= 16
PLATFORM := $(shell ../../scripts/detect-platform)

all: pv status
	@mkdir -p '${ROOT}' && \
	./scripts/build-all '${ROOT}' '${START}' '${START_VERSION}' \
	  '${STOP}' '${STOP_VERSION}' '${PLATFORM}' '${BITS}' '${PARALLEL}'

status:
	@echo && ./scripts/print-status 'start' \
	  'Building package `compiler` for `${PLATFORM}`'

clean-logs:
	rm -f logs/*/*.log

clean: clean-logs
	./scripts/clean-all; \
	find . \( \( -name '*.o' -o -name '*.a' \) -a -type f \) -exec rm -f {} \;

stats: recalculate-stats

recalculate-stats:
	for file in 'logs/${PLATFORM}'/*'-${TS}.log'; do \
	  stats_file="stats/${PLATFORM}/` \
	    basename "$$file" .log | sed 's/-${TS}//g;' \
	  `.count" && \
	  wc -l < "$$file" > "$$stats_file"; \
	done

pv: build-pv

build-pv:
	@(cd ../medic-core && \
	  ${MAKE} ROOT='${ROOT}' \
	    START='pv' STOP='pv' PLATFORM='${PLATFORM}' \
	    SKIP_STATUS=1 BITS='${BITS}' PARALLEL='${PARALLEL}')

