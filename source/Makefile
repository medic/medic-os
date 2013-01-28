
CPU ?= x86
BITS ?= 32
PARALLEL ?= 16

JAVA_VERSION := 1.7.9
VM_TOOLS_VERSION := 9.2.2
MEDIC_CORE_VERSION := 1.2.0

VM_TOOLS_ROOT := /srv/software/vm-tools/v${VM_TOOLS_VERSION}/${CPU}
MEDIC_CORE_ROOT := /srv/software/medic-core/v${MEDIC_CORE_VERSION}/${CPU}

PKG_DIR := ../packages
JAVA_PKG_ROOT := ${PKG_DIR}/java/software/java/v${JAVA_VERSION}/${CPU}
VM_TOOLS_PKG_ROOT := ${PKG_DIR}/vm-tools/software/medic-core/v${MEDIC_CORE_VERSION}/${CPU}
MEDIC_CORE_PKG_ROOT := ${PKG_DIR}/medic-core/software/medic-core/v${MEDIC_CORE_VERSION}/${CPU}

PATH := ${PATH}:${VM_TOOLS_ROOT}/bin
PATH := ${PATH}:${MEDIC_CORE_ROOT}/bin

MAKE ?= make
PMAKE := ${MAKE} -j${PARALLEL}
QMAKE := ${MAKE} --no-print-directory

KERNEL_VERSION := 3.7.3
KERNEL_MODULE_PATH := $(shell readlink -f "../initrd/lib/modules/${KERNEL_VERSION}")


all: build copy finalize


build: build-all

build-all: build-syslinux build-kernel install-kernel-modules build-vm-tools build-busybox build-medic-core


copy: copy-all

copy-all: copy-syslinux copy-kernel copy-vm-tools copy-busybox copy-medic-core set-pax-flags


finalize: finalize-all

finalize-all: finalize-initrd


clean: clean-all

clean-all: clean-syslinux clean-kernel clean-vm-tools clean-busybox clean-medic-core
	rm -f logs/*


pv: build-pv

build-pv:
	@(cd medic-core && echo && \
		${QMAKE} ROOT='${MEDIC_CORE_ROOT}' START='pv' STOP='pv' \
			BITS='${BITS}' PARALLEL='${PARALLEL}')


kernel: build-kernel copy-kernel install-kernel-modules build-vm-tools

clean-kernel:
	(cd linux && ${MAKE} clean)

build-kernel: build-pv
	@(cd linux && \
		cp ../../kernel/boot-logo/logo-medic.ppm \
			./drivers/video/logo/logo_linux_clut224.ppm && \
		cp -H "../../kernel/dot-config-${KERNEL_VERSION}" .config) && \
		./scripts/logger linux \
			'cd linux && ${PMAKE} bzImage && ${PMAKE} modules'

copy-kernel:
	@cp linux/arch/x86/boot/bzImage ../iso/boot/kernel

install-kernel-modules:
	@(cd linux && ${MAKE} modules_install \
		INSTALL_MOD_PATH=../../initrd/ >/dev/null)


vm-tools: build-vm-tools copy-vm-tools

clean-vm-tools:
	(cd vm-tools && ${MAKE} clean)

build-vm-tools: install-kernel-modules
	@(cd vm-tools && ${QMAKE} \
		KERNEL_VERSION='${KERNEL_VERSION}' \
		KERNEL_MODULE_PATH='${KERNEL_MODULE_PATH}' \
		ROOT='${VM_TOOLS_ROOT}' PARALLEL='${PARALLEL}' BITS='${BITS}') && \
	depmod -a -b ../initrd '${KERNEL_VERSION}'

copy-vm-tools:
	@echo -n "Copying files for 'vm-tools'... "
	@(cd vm-tools && \
		./scripts/prepare-dist '${VM_TOOLS_ROOT}' \
		mkdir -p ../../packages/vm-tools/software && \
		rm -rf ../../packages/vm-tools/software/vm-tools && \
		cp -R /srv/software/vm-tools \
			../../packages/vm-tools/software/vm-tools)
	@echo 'done.'


medic-core: build-medic-core copy-medic-core

clean-medic-core:
	rm -rf /srv/software/medic-core && (cd medic-core && ${MAKE} clean)

build-medic-core:
	@(cd medic-core && \
		${QMAKE} all gardenize ROOT='${MEDIC_CORE_ROOT}' \
			BITS='${BITS}' PARALLEL='${PARALLEL}')

copy-medic-core:
	@echo -n "Copying files for 'medic-core'... "
	@(cd medic-core && \
		./scripts/prepare-dist '${MEDIC_CORE_ROOT}' && \
		mkdir -p ../../packages/medic-core/software && \
		rm -rf ../../packages/medic-core/software/medic-core && \
		cp -R /srv/software/medic-core \
			../../packages/medic-core/software/medic-core)
	@echo 'done.'

set-pax-flags:
	@echo -n "Setting executable flags... " && \
	'${MEDIC_CORE_ROOT}/sbin/paxctl' -Czms \
		'${JAVA_PKG_ROOT}/bin/java' 2>/dev/null && \
	'${MEDIC_CORE_ROOT}/sbin/paxctl' -Czms \
		'${MEDIC_CORE_PKG_ROOT}/bin/node' 2>/dev/null && \
	'${MEDIC_CORE_ROOT}/sbin/paxctl' -Czrm \
		'${MEDIC_CORE_PKG_ROOT}/lib/couchdb/bin/couchjs' 2>/dev/null && \
	echo 'done.'

busybox: build-busybox copy-busybox

clean-busybox:
	(cd busybox && ${MAKE} clean)

build-busybox: build-pv
	@(cd busybox && cp ../../busybox/dot-config-1.20.2 .config) && \
	./scripts/logger busybox 'cd busybox && ${PMAKE}'

copy-busybox:
	@echo -n "Copying files for 'busybox'... "
	@(cd busybox && cp busybox ../../initrd/bin/busybox)
	@echo 'done.'


syslinux: build-syslinux copy-syslinux

clean-syslinux:
	(cd syslinux && ${MAKE} clean)

build-syslinux: build-pv
	@./scripts/logger syslinux 'cd syslinux && ${PMAKE}'

copy-syslinux:
	@echo -n "Copying files for 'syslinux'... "
	@(cd syslinux && \
		cp core/isolinux.bin ../../iso/boot/isolinux/isolinux.bin)
	@echo 'done.'


finalize-initrd:
	@for symlink in source build; do \
		rm -f "../initrd/lib/modules/${KERNEL_VERSION}/$$symlink"; \
	done

stats: recalculate-stats

recalculate-stats:
	for file in logs/*"-$(TS).log"; do \
		wc -l < "$$file" \
			> "stats/`basename "$$file" .log | sed 's/-$(TS)//g;'`.count"; \
	done

