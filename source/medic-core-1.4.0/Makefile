
unexport PLATFORM

all:
	@mkdir -p '${ROOT}' && \
	for file in templates/common/* templates/'${PLATFORM}'/*; do \
		if [ -e "$$file" ]; then \
			cp -a "$$file" '${ROOT}'; \
		fi; \
	done && \
	./scripts/build-all '${ROOT}' '${START}' '${START_VERSION}' \
		'${STOP}' '${STOP_VERSION}' '${PLATFORM}' '${BITS}' '${PARALLEL}'

gardenize:
	@./scripts/gardenize-couchdb '${ROOT}'

clean-logs:
	rm -f logs/*.log

clean: clean-logs
	./scripts/clean-all; \
	find . \( \( -name '*.o' -o -name '*.a' \) -a -type f \) -exec rm -f {} \;

stats: recalculate-stats

recalculate-stats:
	for file in logs/*"-${TS}.log"; do \
		wc -l < "$$file" \
			> "stats/`basename "$$file" .log | sed 's/-${TS}//g;'`.count"; \
	done

