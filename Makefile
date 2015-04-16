
# Variables

MAKE ?= make
SHELL := /bin/bash
QMAKE := ${MAKE} --no-print-directory


# Public targets

all: require-root download build

build: reset-time prepare-tree
	@echo >&2
	@echo "`tput bold`Building packages`tput sgr0`" >&2 && echo >&2
	@(cd platform && \
	  export HOME="`readlink -f ../..`" && \
	  source ./.profile && ${QMAKE} compile all)

clean: require-root
	@shopt -u xpg_echo && \
	echo -n 'Cleaning source tree... ' && \
	(cd platform && ${QMAKE} distclean) &>/dev/null && \
	echo 'done.'

distclean: require-root clean-initrd clean-target clean

delete: require-root
	@shopt -u xpg_echo && \
	echo -n 'Deleting downloaded source code... ' && \
	(cd platform && ${QMAKE} delete-downloaded) && \
	echo 'done.'

download: require-root reset-time prepare-tree
	@if ! [ -f platform/status/download.finished ]; then \
	  ${QMAKE} force-download; \
	fi && \
	\
	if ! [ -f platform/status/move.finished ]; then \
	  ${QMAKE} force-move-downloaded; \
	fi

# Private targets

require-root:
	@if [ "`id -u`" -ne 0 ]; then \
	  echo 'Fatal: You must run this build process as root' >&2; \
	  exit 1; \
	fi

clean-target:
	@shopt -u xpg_echo && \
	echo -n 'Cleaning target directory... ' && \
	cd /srv && rm -rf software settings storage && \
	echo 'done.'

clean-initrd:
	@shopt -u xpg_echo && \
	echo -n 'Cleaning initrd... ' && \
	git clean -qf platform/initrd \
	  platform/initrd/*/lib >/dev/null && \
	echo 'done.'

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

