
# Variables

PARALLEL ?= 16
PLATFORM := $(shell ../../scripts/detect-platform)


# Public targets

all: status
	@mkdir -p '${ROOT}' && \
	for file in templates/common/* templates/'${PLATFORM}'/*; do \
		if [ -e "$$file" ]; then \
			cp -a "$$file" '${ROOT}'; \
		fi; \
	done && \
	./scripts/build-all '${ROOT}' '${START}' '${START_VERSION}' \
		'${STOP}' '${STOP_VERSION}' '${PLATFORM}' '${BITS}' '${PARALLEL}'

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


# Private targets

status:
	@if [ -z "${SKIP_STATUS}" ]; then \
		echo && ./scripts/print-status 'start' \
        	'Building package `medic-core` for `${PLATFORM}`'; \
	fi

gardenize:
	@./scripts/gardenize-couchdb '${ROOT}'

clean-logs:
	rm -f logs/*/*.log

