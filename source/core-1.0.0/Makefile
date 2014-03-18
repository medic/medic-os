
PARALLEL ?= 16
PLATFORM := $(shell ../../scripts/detect-platform)

MAKE ?= make
PMAKE := ${MAKE} -j${PARALLEL}
QMAKE := ${MAKE} --no-print-directory

KERNEL_SOURCE_PATH := $(shell readlink -f source/linux)
INITRD_STAGING_PATH := $(shell readlink -f ../../initrd/${PLATFORM})
KERNEL_CONFIG := ../../../../config/kernel/${PLATFORM}/dot-config-${KERNEL_VERSION}


all: build

build: build-all

build-all: status build-pv build-dhcpcd build-syslinux build-kernel install-kernel-modules build-iptables build-busybox


copy: copy-all

copy-all: copy-syslinux copy-kernel copy-iptables copy-busybox


clean: clean-all

clean-all: clean-dhcpcd clean-syslinux clean-kernel clean-iptables clean-busybox
	rm -f logs/*/*


status:
	@echo && ./scripts/print-status 'start' \
	  'Building package `core` for `${PLATFORM}`'

pv: build-pv

build-pv:
	@(cd ../medic-core && \
	  ${QMAKE} ROOT='${MEDIC_CORE_ROOT}' \
	    START='pv' STOP='pv' PLATFORM='${PLATFORM}' \
	    SKIP_STATUS=1 BITS='${BITS}' PARALLEL='${PARALLEL}')


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
	@if [ -h source/dhcpcd ]; then \
	  (cd source/dhcpcd && ${QMAKE} clean); \
	fi


kernel: build-kernel copy-kernel install-kernel-modules

clean-kernel:
	@if [ -h source/linux ]; then \
	  (cd source/linux && ${QMAKE} mrproper); \
	fi

build-kernel: build-pv
	@(cd source/linux && \
	  cp ../../../../config/kernel/common/boot-logo/logo-medic.ppm \
	    ./drivers/video/logo/logo_linux_clut224.ppm && \
	  cp -H "${KERNEL_CONFIG}${KERNEL_EXTRA}" .config) && \
	    ./scripts/logger 'linux' '${PLATFORM}' \
	      'unset MAKEFLAGS && cd source/linux && \
	        ${PMAKE} bzImage && ${PMAKE} modules'

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
	@if [ -h source/busybox ]; then \
	  (cd source/busybox && ${MAKE} clean); \
	fi

build-busybox: build-pv
	@(cd source/busybox && \
	  cp ../../../../config/busybox/dot-config-1.21.1 .config) && \
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
	@if [ -h source/syslinux ]; then \
	  (cd source/syslinux && ${MAKE} clean; \
	    find . -name '*.o' -o -name zbin -exec rm -f {} \;); \
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


iptables: build-iptables copy-iptables

clean-iptables:
	@if [ -h source/iptables ]; then \
	  (cd source/iptables && ${MAKE} clean) \
	fi && \
	rm -rf "${INITRD_STAGING_PATH}/lib/xtables" \
	  "${INITRD_STAGING_PATH}/bin/iptables-"* && \
	for type in ip4tc ip6tc ipq iptc xtables; do \
	  rm -f "${INITRD_STAGING_PATH}/lib/lib$$type".so*; \
	done && \
	for type in xtables iptables ip6tables; do \
	  rm -rf "${INITRD_STAGING_PATH}/sbin/$$type"*; \
	done

build-iptables:
	@./scripts/logger 'iptables' '${PLATFORM}' \
	  "cd source/iptables && \
	    ./configure --prefix=/ \
	      --enable-shared --disable-static \
	      --with-kernel='${KERNEL_SOURCE_PATH}' \
	      --with-pkgconfigdir=/usr/lib/pkgconfig \
	      --enable-libipq --with-xtlibdir=/lib/xtables \
	      --bindir=/bin --sbindir=/sbin --libdir=/lib && \
	   make && \
	   make install DESTDIR='${INITRD_STAGING_PATH}'"

copy-iptables:
	@echo -n "Copying files for 'iptables'... " && \
	for type in ip4tc ip6tc ipq iptc xtables; do \
	  ./scripts/strip-all "${INITRD_STAGING_PATH}/lib/lib$$type".so*; \
	done && \
	./scripts/strip-all \
	  '${INITRD_STAGING_PATH}/lib/xtables' \
	  '${INITRD_STAGING_PATH}/sbin/xtables-multi' && \
	cd source/iptables && \
	rm -rf '${INITRD_STAGING_PATH}/share' \
	  '${INITRD_STAGING_PATH}/include' \
	  '${INITRD_STAGING_PATH}/usr/lib/pkgconfig' \
	  '${INITRD_STAGING_PATH}/lib/libxtables.la' && \
	echo 'done.'


stats: recalculate-stats

recalculate-stats:
	for file in 'logs/${PLATFORM}'/*'-${TS}.log'; do \
	  stats_file="stats/${PLATFORM}/` \
	    basename "$$file" .log | sed 's/-${TS}//g;' \
	  `.count" && \
	  wc -l < "$$file" > "$$stats_file"; \
	done

