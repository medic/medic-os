
MAKE ?= make
PMAKE := ${MAKE} -j${PARALLEL}
QMAKE := ${MAKE} --no-print-directory

PLATFORM ?= x86
KERNEL_CONFIG := ../../../../kernel/${PLATFORM}/dot-config-${KERNEL_VERSION}

all: build

build: build-all

build-all: build-pv build-dhcpcd build-syslinux build-kernel install-kernel-modules build-busybox


copy: copy-all

copy-all: copy-syslinux copy-kernel copy-busybox


clean: clean-all

clean-all: clean-dhcpcd clean-syslinux clean-kernel clean-busybox
	rm -f logs/*/*


pv: build-pv

build-pv:
	@(cd ../medic-core && \
		${QMAKE} ROOT='${MEDIC_CORE_ROOT}' \
			START='pv' STOP='pv' \
			PLATFORM='${PLATFORM}' \
			BITS='${BITS}' PARALLEL='${PARALLEL}')


dhcpcd: build-dhcpcd

build-dhcpcd: build-pv
	@./scripts/logger 'dhcpcd' '${PLATFORM}' \
		'cd source/dhcpcd && \
		./configure --libexecdir=/lib/dhcpcd \
			--dbdir=/run/dhcpcd --sysconfdir=/etc && \
		${MAKE} all CFLAGS="${CFLAGS}" && \
		${MAKE} _proginstall _scriptsinstall _confinstall \
			DESTDIR="../../../../initrd/${PLATFORM}" && \
		cd dhcpcd-hooks && \
		${MAKE} install DESTDIR="../../../../../initrd/${PLATFORM}" \
			SCRIPTS="01-test 02-dump 10-mtu 20-resolv.conf"'

clean-dhcpcd:
	@if [ -f source/dhcpcd ]; then \
		(cd source/dhcpcd && ${QMAKE} clean); \
	fi


kernel: build-kernel copy-kernel install-kernel-modules

clean-kernel:
	if [ -f source/linux ]; then \
		(cd source/linux && ${MAKE} clean); \
	fi

build-kernel: build-pv
	@(cd source/linux && \
		cp ../../../../kernel/common/boot-logo/logo-medic.ppm \
			./drivers/video/logo/logo_linux_clut224.ppm && \
		cp -H "${KERNEL_CONFIG}${KERNEL_EXTRA}" .config) && \
		./scripts/logger 'linux' '${PLATFORM}' \
			'cd source/linux && ${PMAKE} bzImage && ${PMAKE} modules'

copy-kernel:
	@cp source/linux/arch/x86/boot/bzImage \
		"../../images/${PLATFORM}/iso/boot/kernel" && \
	strip -o source/linux/vmlinux-stripped source/linux/vmlinux && \
	mkdir -p "../../images/${PLATFORM}/xen/boot" && \
	gzip -c9 source/linux/vmlinux-stripped \
		> "../../images/${PLATFORM}/xen/boot/kernel"

install-kernel-modules:
	@rm -rf "../../initrd/${PLATFORM}/lib/modules"/* && \
	(cd source/linux && ${MAKE} modules_install \
		INSTALL_MOD_PATH="../../../../initrd/${PLATFORM}" &>/dev/null)


busybox: build-busybox copy-busybox

clean-busybox:
	@if [ -f source/busybox ]; then \
		(cd source/busybox && ${MAKE} clean); \
	fi

build-busybox: build-pv
	@(cd source/busybox && cp ../../../../busybox/dot-config-1.21.1 .config) && \
	./scripts/logger 'busybox' '${PLATFORM}' \
		'cd source/busybox && ${PMAKE}'

copy-busybox:
	@echo -n "Copying files for 'busybox'... " && \
	mkdir -p "../../initrd/${PLATFORM}/bin" && \
	(cd source/busybox && cp busybox \
		../../../../initrd/${PLATFORM}/bin/busybox) && \
	echo 'done.'


syslinux: build-syslinux copy-syslinux

clean-syslinux:
	@if [ -f source/syslinux ]; then \
	  (cd source/syslinux && ${MAKE} clean); \
	fi

build-syslinux: build-pv
	@./scripts/logger 'syslinux' '${PLATFORM}' \
		'cd source/syslinux && ${PMAKE} bios MAKEFLAGS='

copy-syslinux:
	@echo -n "Copying files for 'syslinux'... " && \
	(cd source/syslinux && \
	  cp bios/core/isolinux.bin \
			bios/com32/elflink/ldlinux/ldlinux.c32 \
		  "../../../../images/${PLATFORM}/iso/boot/isolinux/") && \
	echo 'done.'


stats: recalculate-stats

recalculate-stats:
	for file in 'logs/${PLATFORM}'/*'-${TS}.log'; do \
		stats_file="stats/${PLATFORM}/` \
			basename "$$file" .log | sed 's/-${TS}//g;' \
		`.count" && \
		wc -l < "$$file" > "$$stats_file"; \
	done

