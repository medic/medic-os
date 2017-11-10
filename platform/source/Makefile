
# Variables

BITS ?= 64
PARALLEL ?= 16
PLATFORM := $(shell ../scripts/detect-platform)

JAVA_VERSION ?= 1.7.80
VM_TOOLS_VERSION ?= 9.4.6
MEDIC_CORE_VERSION ?= 1.7.0
MEDIC_RDBMS_VERSION ?= 9.5.3
MEDIC_COUCH2PG_VERSION ?= 1.0.0

VM_TOOLS_ROOT ?= /srv/software/vm-tools/v${VM_TOOLS_VERSION}/${PLATFORM}
MEDIC_CORE_ROOT ?= /srv/software/medic-core/v${MEDIC_CORE_VERSION}/${PLATFORM}
MEDIC_COUCH2PG_ROOT ?= /srv/software/medic-couch2pg/v${MEDIC_COUCH2PG_VERSION}
MEDIC_RDBMS_ROOT ?= /srv/software/medic-rdbms/v${MEDIC_RDBMS_VERSION}/${PLATFORM}

PKG_DIR ?= ../packages
JAVA_PKG_ROOT ?= ${PKG_DIR}/java/software/java/v${JAVA_VERSION}/${PLATFORM}
VM_TOOLS_PKG_ROOT ?= ${PKG_DIR}/vm-tools/software/vm-tools/v${VM_TOOLS_VERSION}/${PLATFORM}
MEDIC_CORE_PKG_ROOT ?= ${PKG_DIR}/medic-core/software/medic-core/v${MEDIC_CORE_VERSION}/${PLATFORM}

MAKE ?= make
SHELL := /bin/bash
PMAKE := ${MAKE} -j${PARALLEL}
QMAKE := ${MAKE} --no-print-directory

KERNEL_VERSION := 4.9.17
KERNEL_MODULE_PATH := ${CURDIR}/../initrd/${PLATFORM}/lib/modules/${KERNEL_VERSION}


# Meta-targets

all: build copy finalize

build: build-all
build-all: build-core build-vm-tools build-medic-core build-medic-rdbms build-medic-couch2pg

copy: copy-all
copy-all: copy-core copy-vm-tools copy-medic-core copy-medic-rdbms copy-medic-couch2pg set-pax-flags

finalize: finalize-all
finalize-all: verify-packages finalize-initrd

clean: clean-all
clean-all: clean-core clean-vm-tools clean-medic-core clean-medic-rdbms clean-medic-couch2pg clean-java clean-compiler


# Public targets

compiler: build-compiler

build-compiler:
	@(cd compiler && ${QMAKE} \
	  ROOT='${COMPILER_ROOT}' BITS='${BITS}' \
	  PLATFORM='${PLATFORM}' PARALLEL='${PARALLEL}')

clean-compiler:
	(cd compiler && ${MAKE} clean)


core: build-core copy-core

build-core:
	@source medic-core/scripts/include/builder && \
	builder_setup_cache; \
	(cd core && ${QMAKE} \
	  COMPILER_ROOT='${COMPILER_ROOT}' \
	  PATH="${MEDIC_CORE_ROOT}/bin:$$PATH" \
	  BITS='${BITS}' KERNEL_VERSION='${KERNEL_VERSION}' \
	  MEDIC_CORE_ROOT='${MEDIC_CORE_ROOT}' PLATFORM='${PLATFORM}' \
	  KERNEL_MODULE_PATH='${KERNEL_MODULE_PATH}' PARALLEL='${PARALLEL}')

copy-core:
	@(cd core && \
	  ${MAKE} copy PLATFORM='${PLATFORM}')

clean-core:
	(cd core && ${MAKE} clean)


vm-tools: build-vm-tools copy-vm-tools

clean-vm-tools:
	@rm -rf /srv/software/vm-tools && \
	(cd vm-tools && ${MAKE} clean)

build-vm-tools:
	@(cd vm-tools && ${QMAKE} \
	  PLATFORM='${PLATFORM}' \
	  KERNEL_MODULE_PATH='${KERNEL_MODULE_PATH}' \
	  BITS='${BITS}' KERNEL_VERSION='${KERNEL_VERSION}' \
	  ROOT='${VM_TOOLS_ROOT}' PARALLEL='${PARALLEL}') && \
	\
	depmod -a -b '../initrd/${PLATFORM}' '${KERNEL_VERSION}'

copy-vm-tools:
	@echo -n "Copying files for 'vm-tools'... "
	@(cd vm-tools && \
	  ./scripts/prepare-dist '${VM_TOOLS_ROOT}' && \
	  mkdir -p ../../packages/vm-tools/software && \
	  rm -rf ../../packages/vm-tools/software/vm-tools && \
	  cp -R /srv/software/vm-tools \
	    ../../packages/vm-tools/software/vm-tools)
	@echo 'done.'


medic-core: build-medic-core copy-medic-core

clean-medic-core:
	@rm -rf /srv/software/medic-core && \
	(cd medic-core && ${MAKE} clean)

build-medic-core:
	@(cd medic-core && \
	  ./scripts/unprepare-dist '${MEDIC_CORE_ROOT}' && \
	  ${QMAKE} all preload \
	    BITS='${BITS}' PLATFORM='${PLATFORM}' \
	    ROOT='${MEDIC_CORE_ROOT}' PARALLEL='${PARALLEL}')

copy-medic-core:
	@echo -n "Copying files for 'medic-core'... "
	@(cd medic-core && \
	  ./scripts/prepare-dist '${MEDIC_CORE_ROOT}' && \
	  mkdir -p ../../packages/medic-core/software && \
	  rm -rf ../../packages/medic-core/software/medic-core && \
	  cp -R /srv/software/medic-core \
	    ../../packages/medic-core/software/medic-core)
	@echo 'done.'

preload-medic-core:
	@(cd medic-core && \
	  ${QMAKE} preload ROOT='${MEDIC_CORE_ROOT}')


medic-rdbms: build-medic-rdbms copy-medic-rdbms

clean-medic-rdbms:
	@rm -rf /srv/software/medic-rdbms && \
	(cd medic-rdbms && ${MAKE} clean)

build-medic-rdbms:
	@(cd medic-rdbms && ${QMAKE} all \
	    BITS='${BITS}' PLATFORM='${PLATFORM}' \
	    ROOT='${MEDIC_RDBMS_ROOT}' PARALLEL='${PARALLEL}')

copy-medic-rdbms:
	@echo -n "Copying files for 'medic-rdbms'... "
	@(cd medic-rdbms && \
	  ./scripts/prepare-dist '${MEDIC_RDBMS_ROOT}' && \
	  mkdir -p ../../packages/medic-rdbms/software && \
	  rm -rf ../../packages/medic-rdbms/software/medic-rdbms && \
	  cp -R /srv/software/medic-rdbms \
	    ../../packages/medic-rdbms/software/medic-rdbms)
	@echo 'done.'


medic-couch2pg: build-medic-couch2pg copy-medic-couch2pg

clean-medic-couch2pg:
	(cd medic-couch2pg && ${MAKE} clean)

build-medic-couch2pg:
	@(cd medic-couch2pg && ${QMAKE} all \
	    ROOT='${MEDIC_COUCH2PG_ROOT}' \
	    COMPILER_ROOT='${COMPILER_ROOT}' \
	    MEDIC_CORE_ROOT='${MEDIC_CORE_ROOT}')

copy-medic-couch2pg:
	@echo -n "Copying files for 'medic-couch2pg'... "
	@(cd medic-couch2pg && \
	  ./scripts/prepare-dist '${MEDIC_COUCH2PG_ROOT}' && \
	  mkdir -p ../../packages/medic-couch2pg/software && \
	  rm -rf ../../packages/medic-couch2pg/software/medic-couch2pg && \
	  cp -R /srv/software/medic-couch2pg \
	    ../../packages/medic-couch2pg/software/medic-couch2pg)
	@echo 'done.'


# Private targets

rebuild-kernel:
	@(cd core && \
	  ${QMAKE} clean-kernel &>/dev/null && \
	  ${QMAKE} kernel \
	    PLATFORM='${PLATFORM}' \
	    PATH="${MEDIC_CORE_ROOT}/bin:$$PATH" \
	    MEDIC_CORE_ROOT='${MEDIC_CORE_ROOT}' \
	    KERNEL_VERSION='${KERNEL_VERSION}' KERNEL_EXTRA='${KERNEL_EXTRA}' \
	    KERNEL_MODULE_PATH='${KERNEL_MODULE_PATH}' PARALLEL='${PARALLEL}')

copy-java-template: clean-java
	@echo -n "Copying package template for 'java'... " && \
	rm -rf ../packages/java/software && \
	cp -a "../templates/java/${PLATFORM}/software" ../packages/java/software && \
	echo 'done.'

clean-java:
	@rm -rf ../packages/java/software

set-pax-flags: copy-java-template
	@echo -n "Setting executable flags... " && \
	'${MEDIC_CORE_ROOT}/sbin/paxctl' -Czms \
	  '${JAVA_PKG_ROOT}/bin/java' 2>/dev/null && \
	'${MEDIC_CORE_ROOT}/sbin/paxctl' -Czms \
	  '${MEDIC_CORE_PKG_ROOT}/bin/node' 2>/dev/null && \
	'${MEDIC_CORE_ROOT}/sbin/paxctl' -Czms \
	  '${MEDIC_CORE_PKG_ROOT}/bin/lrzip' 2>/dev/null && \
	'${MEDIC_CORE_ROOT}/sbin/paxctl' -Czrm \
	  '${MEDIC_CORE_PKG_ROOT}/lib/couchdb/bin/couchjs' 2>/dev/null && \
	echo 'done.'

finalize-initrd:
	@for symlink in source build; do \
	  rm -f "../initrd/${PLATFORM}/lib/modules/${KERNEL_VERSION}/$$symlink" \
	    || exit "$$?"; \
	done

verify-packages:
	@echo -n "Verifying package contents... " && \
	./scripts/verify-packages "${MEDIC_CORE_ROOT}" && \
	echo 'done.'

