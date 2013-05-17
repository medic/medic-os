
BITS ?= 32
ARCH ?= x86
PARALLEL ?= 16

JAVA_VERSION := 1.7.21
VM_TOOLS_VERSION := 9.2.2
MEDIC_CORE_VERSION := 1.2.2

VM_TOOLS_ROOT := /srv/software/vm-tools/v${VM_TOOLS_VERSION}/${ARCH}
MEDIC_CORE_ROOT := /srv/software/medic-core/v${MEDIC_CORE_VERSION}/${ARCH}

PKG_DIR := ../packages
JAVA_PKG_ROOT := ${PKG_DIR}/java/software/java/v${JAVA_VERSION}/${ARCH}
VM_TOOLS_PKG_ROOT := ${PKG_DIR}/vm-tools/software/medic-core/v${MEDIC_CORE_VERSION}/${ARCH}
MEDIC_CORE_PKG_ROOT := ${PKG_DIR}/medic-core/software/medic-core/v${MEDIC_CORE_VERSION}/${ARCH}
PATH := ${PATH}:${VM_TOOLS_ROOT}/bin
PATH := ${PATH}:${MEDIC_CORE_ROOT}/bin

MAKE ?= make
PMAKE := ${MAKE} -j${PARALLEL}
QMAKE := ${MAKE} --no-print-directory

CFLAGS += -Os -fstack-protector-all

KERNEL_VERSION := 3.8.5
KERNEL_MODULE_PATH := $(shell readlink -f "../initrd/${ARCH}/lib/modules/${KERNEL_VERSION}")


all: build copy finalize


build: build-all

build-all: build-dhcpcd build-syslinux build-kernel install-kernel-modules build-vm-tools build-busybox build-medic-core


copy: copy-all

copy-all: copy-syslinux copy-kernel copy-vm-tools copy-busybox copy-medic-core set-pax-flags


finalize: finalize-all

finalize-all: finalize-initrd


clean: clean-all

clean-all: clean-dhcpcd clean-syslinux clean-kernel clean-vm-tools clean-busybox clean-medic-core
	rm -f logs/*


pv: build-pv

build-pv:
	@(cd medic-core && echo && \
		${QMAKE} ROOT='${MEDIC_CORE_ROOT}' START='pv' STOP='pv' \
			BITS='${BITS}' PARALLEL='${PARALLEL}')


dhcpcd: build-dhcpcd

build-dhcpcd: build-pv
	@./scripts/logger dhcpcd \
		'cd dhcpcd && \
		./configure --libexecdir=/lib/dhcpcd \
			--dbdir=/run/dhcpcd --sysconfdir=/etc && \
		${MAKE} all CFLAGS="${CFLAGS}" && \
		${MAKE} _proginstall _scriptsinstall _confinstall \
			DESTDIR="../../initrd/${ARCH}" && \
		cd dhcpcd-hooks && \
		${MAKE} install DESTDIR="../../../initrd/${ARCH}" \
			SCRIPTS="01-test 02-dump 10-mtu 20-resolv.conf"'

clean-dhcpcd:
	@(cd dhcpcd && ${QMAKE} clean)


kernel: build-kernel copy-kernel install-kernel-modules build-vm-tools

clean-kernel:
	(cd linux && ${MAKE} clean)

build-kernel: build-pv
	@(cd linux && \
		cp ../../kernel/boot-logo/logo-medic.ppm \
			./drivers/video/logo/logo_linux_clut224.ppm && \
		cp -H "../../kernel/dot-config-${KERNEL_VERSION}-grsec" .config) && \
		./scripts/logger linux \
			'cd linux && ${PMAKE} bzImage && ${PMAKE} modules'

copy-kernel:
	@cp linux/arch/x86/boot/bzImage ../images/x86/iso/boot/kernel

install-kernel-modules:
	@(cd linux && ${MAKE} modules_install \
		INSTALL_MOD_PATH="../../initrd/${ARCH}" >/dev/null)


vm-tools: build-vm-tools copy-vm-tools

clean-vm-tools:
	(cd vm-tools && ${MAKE} clean)

build-vm-tools: build-pv install-kernel-modules
	@(cd vm-tools && ${QMAKE} \
		KERNEL_VERSION='${KERNEL_VERSION}' \
		KERNEL_MODULE_PATH='${KERNEL_MODULE_PATH}' \
		ROOT='${VM_TOOLS_ROOT}' PARALLEL='${PARALLEL}' BITS='${BITS}') && \
	depmod -a -b '../initrd/${ARCH}' '${KERNEL_VERSION}'

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
	rm -rf /srv/software/medic-core && (cd medic-core && ${MAKE} clean)

build-medic-core: build-pv
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
	echo "${MEDIC_CORE_ROOT}" && \
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
	@echo -n "Copying files for 'busybox'... " && \
	mkdir -p "../initrd/${ARCH}/bin" && \
	(cd busybox && cp busybox ../../initrd/${ARCH}/bin/busybox) && \
	echo 'done.'


syslinux: build-syslinux copy-syslinux

clean-syslinux:
	(cd syslinux && ${MAKE} clean)

build-syslinux: build-pv
	@./scripts/logger syslinux 'cd syslinux && ${PMAKE}'

copy-syslinux:
	@echo -n "Copying files for 'syslinux'... " && \
	(cd syslinux && \
		cp core/isolinux.bin ../../images/x86/iso/boot/isolinux/isolinux.bin) && \
	echo 'done.'


finalize-initrd:
	@for symlink in source build; do \
		rm -f "../initrd/${ARCH}/lib/modules/${KERNEL_VERSION}/$$symlink"; \
	done

stats: recalculate-stats

recalculate-stats:
	for file in logs/*"-$(TS).log"; do \
		wc -l < "$$file" \
			> "stats/`basename "$$file" .log | sed 's/-$(TS)//g;'`.count"; \
	done

