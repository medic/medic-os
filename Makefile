
# Variables

MAKE ?= make
SHELL := /bin/bash
QMAKE := ${MAKE} --no-print-directory


# Public targets

all: require-root download build docker
repackage: do-repackage docker

bootinit:
	@cd platform && ${QMAKE} bootinit

build: prepare-tree
	@echo >&2
	@echo "`tput bold`Building packages`tput sgr0`" >&2 && echo >&2
	@(cd platform && \
	  export HOME="`readlink -f ../.. 2>/dev/null || realpath ../..`" && \
	  source ./.profile && ${QMAKE} build all)

clean: require-root
	@printf 'Cleaning source tree... ' && \
	(cd platform && ${QMAKE} distclean) &>/dev/null && \
	echo 'done.'

distclean: require-root clean-initrd clean-target clean

delete: require-root
	@printf 'Deleting downloaded source code... ' && \
	(cd platform && ${QMAKE} delete-downloaded) && \
	echo 'done.'

download: require-root prepare-tree
	@if ! [ -f platform/status/download.finished ]; then \
	  ${QMAKE} force-download; \
	fi && \
	\
	if ! [ -f platform/status/move.finished ]; then \
	  ${QMAKE} force-move-downloaded; \
	fi

# Legacy command. We now prepare-tree inside ubuntu container and not in a VM.
# After files are modified on local filesystem, then run docker build from OSX
#docker:
#	@docker-compose build

# Private targets

do-repackage: prepare-tree
	@(cd platform && \
	  ${QMAKE} clean &>/dev/null && \
	  ${QMAKE} copy all)

require-root:
	@if [ "`id -u`" -ne 0 ]; then \
	  echo 'Fatal: You must run this build process as root' >&2; \
	  exit 1; \
	fi

clean-target:
	@printf 'Cleaning target directory... ' && \
	if [ -d /srv ]; then \
	  cd /srv && rm -rf software settings storage; \
	fi && \
	echo 'done.'

clean-initrd:
	@printf 'Cleaning initrd... ' && \
	git clean -qf platform/initrd \
	  platform/initrd/*/lib >/dev/null && \
	echo 'done.'

prepare-tree:
	@printf 'Preparing source tree... ' && \
	./scripts/prepare-tree && \
	echo 'done.'

force-download:
	@echo >&2
	@echo "`tput bold`Retrieving source code`tput sgr0`" >&2 && echo >&2
	@(cd platform && ${QMAKE} download)

force-move-downloaded:
	@(cd platform && ${QMAKE} move-downloaded)

