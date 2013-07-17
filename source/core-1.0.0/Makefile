
MAKE ?= make
PMAKE := ${MAKE} -j${PARALLEL}
QMAKE := ${MAKE} --no-print-directory

KERNEL_CONFIG := ../../../kernel/${PLATFORM}/dot-config-${KERNEL_VERSION}-grsec


all: build

build: build-all

build-all: build-pv build-dhcpcd build-syslinux build-kernel install-kernel-modules build-busybox


copy: copy-all

copy-all: copy-syslinux copy-kernel copy-busybox


clean: clean-all

clean-all: clean-dhcpcd clean-syslinux clean-kernel clean-busybox
	rm -f logs/*


pv: build-pv

build-pv:
	@(cd ../medic-core && echo && \
		${QMAKE} ROOT='${MEDIC_CORE_ROOT}' \
			START='pv' STOP='pv' \
			PLATFORM='${PLATFORM}' \
			BITS='${BITS}' PARALLEL='${PARALLEL}')


dhcpcd: build-dhcpcd

build-dhcpcd: build-pv
	@./scripts/logger dhcpcd \
		'cd dhcpcd && \
		./configure --libexecdir=/lib/dhcpcd \
			--dbdir=/run/dhcpcd --sysconfdir=/etc && \
		${MAKE} all CFLAGS="${CFLAGS}" && \
		${MAKE} _proginstall _scriptsinstall _confinstall \
			DESTDIR="../../../initrd/${PLATFORM}" && \
		cd dhcpcd-hooks && \
		${MAKE} install DESTDIR="../../../../initrd/${PLATFORM}" \
			SCRIPTS="01-test 02-dump 10-mtu 20-resolv.conf"'

clean-dhcpcd:
	@(cd dhcpcd && ${QMAKE} clean)


kernel: build-kernel copy-kernel install-kernel-modules

clean-kernel:
	(cd linux && ${MAKE} clean)

build-kernel: build-pv
	@(cd linux && \
		cp ../../../kernel/boot-logo/logo-medic.ppm \
			./drivers/video/logo/logo_linux_clut224.ppm && \
		cp -H "${KERNEL_CONFIG}" .config) && \
		./scripts/logger linux \
			'cd linux && ${PMAKE} bzImage && ${PMAKE} modules'

copy-kernel:
	@cp linux/arch/x86/boot/bzImage \
		"../../images/${PLATFORM}/iso/boot/kernel" && \
	strip linux/vmlinux && \
	mkdir -p "../../images/${PLATFORM}/xen/boot" && \
	gzip -c9 linux/vmlinux > "../../images/${PLATFORM}/xen/boot/kernel"

install-kernel-modules:
	@(cd linux && ${MAKE} modules_install \
		INSTALL_MOD_PATH="../../../initrd/${PLATFORM}" &>/dev/null)


busybox: build-busybox copy-busybox

clean-busybox:
	(cd busybox && ${MAKE} clean)

build-busybox: build-pv
	@(cd busybox && cp ../../../busybox/dot-config-1.20.2 .config) && \
	./scripts/logger busybox 'cd busybox && ${PMAKE}'

copy-busybox:
	@echo -n "Copying files for 'busybox'... " && \
	mkdir -p "../../initrd/${PLATFORM}/bin" && \
	(cd busybox && cp busybox \
		../../../initrd/${PLATFORM}/bin/busybox) && \
	echo 'done.'


syslinux: build-syslinux copy-syslinux

clean-syslinux:
	(cd syslinux && ${MAKE} clean)

build-syslinux: build-pv
	@./scripts/logger syslinux 'cd syslinux && ${PMAKE} MAKEFLAGS='

copy-syslinux:
	@echo -n "Copying files for 'syslinux'... " && \
	(cd syslinux && \
		cp core/isolinux.bin ../../../images/x86/iso/boot/isolinux/isolinux.bin) && \
	echo 'done.'


stats: recalculate-stats

recalculate-stats:
	for file in logs/*"-$(TS).log"; do \
		wc -l < "$$file" \
			> "stats/`basename "$$file" .log | sed 's/-$(TS)//g;'`.count"; \
	done

