
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
	@shopt -u xpg_echo && \
	echo -n 'Cleaning source tree... ' && \
	(cd platform && ${QMAKE} distclean) &>/dev/null && \
	echo 'done.'

distclean: clean clean-target

delete:
	@shopt -u xpg_echo && \
	echo -n 'Deleting downloaded source code... ' && \
	(cd platform && ${QMAKE} delete-downloaded) && \
	echo 'done.'

download: reset-time prepare-tree
	@if ! [ -f platform/status/download.finished ]; then \
	  ${QMAKE} force-download; \
	fi && \
	\
	if ! [ -f platform/status/move.finished ]; then \
	  ${QMAKE} force-move-downloaded; \
	fi

# Private targets

clean-target:
	@cd /srv && \
	rm -rf software settings storage

reset-time:
	@shopt -u xpg_echo && \
	echo -n 'Synchronizing system time... ' && \
	ntpdate -u pool.ntp.org >/dev/null && \
	echo 'done.'

prepare-tree:
	@shopt -u xpg_echo && \
	echo -n 'Preparing source tree... ' && \
	./scripts/prepare-tree && \
	echo 'done.'

force-download:
	@echo >&2
	@echo "`tput bold`Retrieving source code`tput sgr0`" >&2 && echo >&2
	@(cd platform && ${QMAKE} download)

force-move-downloaded:
	@(cd platform && ${QMAKE} move-downloaded)

