
# Variables

PARALLEL ?= 16
PLATFORM := $(shell ../scripts/detect-platform)

JAVA_VERSION ?= 1.7.65
VM_TOOLS_VERSION ?= 9.4.0
MEDIC_CORE_VERSION ?= 1.6.0

VM_TOOLS_ROOT ?= /srv/software/vm-tools/v${VM_TOOLS_VERSION}/${PLATFORM}
MEDIC_CORE_ROOT ?= /srv/software/medic-core/v${MEDIC_CORE_VERSION}/${PLATFORM}

PKG_DIR ?= ../packages
JAVA_PKG_ROOT ?= ${PKG_DIR}/java/software/java/v${JAVA_VERSION}/${PLATFORM}
VM_TOOLS_PKG_ROOT ?= ${PKG_DIR}/vm-tools/software/vm-tools/v${VM_TOOLS_VERSION}/${PLATFORM}
MEDIC_CORE_PKG_ROOT ?= ${PKG_DIR}/medic-core/software/medic-core/v${MEDIC_CORE_VERSION}/${PLATFORM}

MAKE ?= make
PMAKE := ${MAKE} -j${PARALLEL}
QMAKE := ${MAKE} --no-print-directory

CFLAGS += -Os -fstack-protector-all

KERNEL_VERSION := 3.10.54
KERNEL_MODULE_PATH := ${CURDIR}/../initrd/${PLATFORM}/lib/modules/${KERNEL_VERSION}


# Meta-targets

all: build copy finalize

build: build-all
build-all: build-core build-vm-tools build-medic-core

copy: copy-all
copy-all: copy-core copy-medic-core copy-vm-tools set-pax-flags

finalize: finalize-all
finalize-all: verify-packages finalize-initrd

clean: clean-all
clean-all: clean-core clean-vm-tools clean-medic-core clean-java clean-compiler


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
	@(cd core && ${QMAKE} \
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
	@rm -rf /srv/software/medic-core && (cd medic-core && ${MAKE} clean)

build-medic-core:
	@(cd medic-core && ${QMAKE} all gardenize \
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

rebuild-vm-tools-modules:
	@echo -n "Rebuilding 'open-vm-tools' kernel modules... " && \
	(cd vm-tools/source/open-vm-tools/modules && \
	  ${QMAKE} clean &>/dev/null && \
	  ${QMAKE} all install &>/dev/null) || exit "$$?"; \
	depmod -a -b '../initrd/${PLATFORM}' '${KERNEL_VERSION}' && \
	echo 'done.'

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
	  rm -f "../initrd/${PLATFORM}/lib/modules/${KERNEL_VERSION}/$$symlink"; \
	done

verify-packages:
	@echo -n "Verifying package contents... " && \
	./scripts/verify-packages "${MEDIC_CORE_ROOT}" && \
	echo 'done.'

