
BITS ?= 32
PARALLEL ?= 16

ROOT := /srv/software/medic-core/v1.2.0/x86
PATH := ${PATH}:${ROOT}/bin

MAKE ?= make
PMAKE := ${MAKE} -j${PARALLEL}
QMAKE := ${MAKE} --no-print-directory

KERNEL_VERSION := 3.6.11
KERNEL_MODULE_PATH := $(shell readlink -f "../initrd/lib/modules/${KERNEL_VERSION}")


all: build copy


build: build-all

build-all: build-syslinux build-kernel build-vm-tools build-busybox build-medic-core


copy: copy-all

copy-all: copy-syslinux copy-kernel copy-vm-tools copy-busybox copy-medic-core


clean: clean-all

clean-all: clean-syslinux clean-kernel clean-vm-tools clean-busybox clean-medic-core
	rm -f logs/timestamp


pv: build-pv

build-pv:
	@(cd medic-core && echo && \
		${QMAKE} ROOT='${ROOT}' START='pv' STOP='pv' \
			BITS='${BITS}' PARALLEL='${PARALLEL}')


kernel: build-kernel copy-kernel

clean-kernel:
	(cd linux && ${MAKE} clean)

build-kernel: build-pv
	@(cd linux && \
		cp ../../kernel/boot-logo/logo_medic_clut224.ppm \
			./drivers/video/logo/logo_linux_clut224.ppm && \
		cp "../../kernel/dot-config-${KERNEL_VERSION}" .config) && \
		./scripts/logger linux \
			'cd linux && ${PMAKE} bzImage && ${PMAKE} modules'

copy-kernel:
	@cp linux/arch/x86/boot/bzImage ../iso/boot/kernel && \
	(cd linux && ${MAKE} modules_install \
		INSTALL_MOD_PATH=../../initrd/ >/dev/null)



vm-tools: build-vm-tools copy-vm-tools

clean-vm-tools:
	(cd open-vm-tools && make clean; true)

build-vm-tools: copy-kernel
	@(cd open-vm-tools && \
		../scripts/logger ../vm-tools \
		    	'./configure --with-kernel-release="${KERNEL_VERSION}" \
			 --prefix=/srv/software/vm-tools/9.2.2/x86 \
			 --with-linuxdir="${KERNEL_MODULE_PATH}" --without-x \
			 --without-gtk2 --without-pam --without-procps \
			 --without-icu --without-dnet && make && make install')

copy-vm-tools:
	@echo -n "Copying files for 'vm-tools'... "
	@(cd medic-core && \
		mkdir -p ../../packages/vm-tools/software && \
		rm -rf ../../packages/vm-tools/software/vmware-tools && \
		cp -R /srv/software/vmware-tools \
			../../packages/vmware-tools/software/vmware-tools)
	@echo 'done.'


medic-core: build-medic-core copy-medic-core

clean-medic-core:
	rm -rf /srv/software/medic-core && (cd medic-core && ${MAKE} clean)

build-medic-core:
	@(cd medic-core && \
		${QMAKE} ROOT='${ROOT}' BITS='${BITS}' PARALLEL='${PARALLEL}')

copy-medic-core:
	@echo -n "Copying files for 'medic-core'... "
	@(cd medic-core && ./scripts/prepare-dist && \
		mkdir -p ../../packages/medic-core/software && \
		rm -rf ../../packages/medic-core/software/medic-core && \
		cp -R /srv/software/medic-core \
			../../packages/medic-core/software/medic-core)
	@echo 'done.'


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


stats: recalculate-stats

recalculate-stats:
	for file in logs/*"-$(TS).log"; do \
		wc -l < "$$file" \
			> "stats/`basename "$$file" .log | sed 's/-$(TS)//g;'`.count"; \
	done

