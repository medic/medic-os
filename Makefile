
# Variables

MAKE ?= make
QMAKE := ${MAKE} --no-print-directory


# Public targets

all: download build

build: reset-time prepare-tree
	@echo >&2
	@echo "`tput bold`Building packages`tput sgr0`" >&2 && echo >&2
	@(cd platform && \
	  export HOME="`readlink -f ../..`" && \
	  source ./.profile && ${QMAKE} compile all)

clean:
	@echo -n 'Cleaning source tree... ' && \
	(cd platform && ${QMAKE} distclean) &>/dev/null && \
	echo 'done.'

distclean: clean clean-target

delete:
	@echo -n 'Deleting downloaded source code... ' && \
	(cd platform && ${QMAKE} delete-downloaded) && \
	echo 'done.'

download: reset-time prepare-tree
	@if ! [ -f status/download.finished ]; then \
	  ${QMAKE} force-download; \
	fi && \
	\
	if ! [ -f status/move.finished ]; then \
	  ${QMAKE} force-move-downloaded; \
	fi


# Private targets

clean-target:
	@cd /srv && \
	rm -rf software settings storage

reset-time:
	@echo -n 'Synchronizing system time... ' && \
	ntpdate pool.ntp.org >/dev/null && \
	echo 'done.'

prepare-tree:
	@echo -n 'Preparing source tree... ' && \
	./scripts/prepare-tree && \
	echo 'done.'

force-download:
	@echo >&2
	@echo "`tput bold`Retrieving source code`tput sgr0`" >&2 && echo >&2
	@(cd platform && make download)

force-move-downloaded:
	@(cd platform && make move-downloaded)

