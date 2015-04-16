
# Variables

PARALLEL ?= 16
PLATFORM := $(shell ../../scripts/detect-platform)

MAKE ?= make
SHELL := /bin/bash
PMAKE := ${MAKE} -j${PARALLEL}
QMAKE := ${MAKE} --no-print-directory

CFLAGS += -Os -fstack-protector-all

KERNEL_SOURCE_PATH := $(shell readlink -f source/linux)
INITRD_PATH := $(shell readlink -f ../../initrd/${PLATFORM})
STAGING_PATH := $(shell readlink -f ../../staging/software/${PLATFORM})
KERNEL_CONFIG := ../../../../config/kernel/${PLATFORM}/dot-config-${KERNEL_VERSION}


# Meta-targets

all: build

build: build-all
build-all: status build-pv build-dhcpcd build-syslinux build-eudev build-sudo build-kernel install-kernel-modules build-iptables build-e2fsprogs build-busybox build-glibc

copy: copy-all
copy-all: copy-dhcpcd copy-syslinux copy-eudev copy-sudo copy-kernel copy-iptables copy-e2fsprogs copy-busybox copy-glibc

clean: clean-all
clean-all: clean-dhcpcd clean-syslinux clean-eudev clean-sudo clean-kernel clean-iptables clean-xfsprogs clean-busybox clean-glibc clean-logs


# Public targets

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
	      --dbdir=/var/run/dhcpcd --sysconfdir=/etc && \
	    ${PMAKE} all CFLAGS="${CFLAGS}" && \
	    ${MAKE} _proginstall _scriptsinstall _confinstall \
	      DESTDIR="${INITRD_PATH}" && \
	    cd dhcpcd-hooks && \
	      ${MAKE} install DESTDIR="${INITRD_PATH}" \
	        SCRIPTS="01-test 02-dump 10-mtu 20-resolv.conf"'

copy-dhcpcd:
	@true

clean-dhcpcd:
	@if [ -h source/dhcpcd ]; then \
	  (cd source/dhcpcd && ${QMAKE} clean); \
	fi


eudev: build-eudev

build-eudev: build-pv
	@./scripts/logger 'eudev' '${PLATFORM}' \
	  'cd source/eudev && autoreconf -i && \
		  ./configure --enable-shared --disable-static \
		    --prefix=/usr --bindir=/sbin --sbindir=/sbin \
			--libdir=/usr/lib --sysconfdir=/etc --libexecdir=/lib \
			--with-rootprefix= --with-rootlibdir=/lib --enable-split-usr \
			--enable-rule_generator --disable-keymap --disable-gudev \
			--disable-introspection --disable-gtk-doc-html && \
		  ${PMAKE} && \
		  ${MAKE} install DESTDIR="${INITRD_PATH}"'

copy-eudev:
	@true

clean-eudev:
	@if [ -h source/eudev ]; then \
	  (cd source/eudev && ${QMAKE} clean); \
	fi


sudo: build-sudo

build-sudo: build-pv
	@./scripts/logger 'sudo' '${PLATFORM}' \
	  'cd source/sudo && \
	    ./configure --prefix=/usr --libexecdir=/usr/lib \
		  --disable-static --enable-shared --disable-zlib \
		  --without-insults --with-secure-path --with-env-editor \
		  --without-sendmail --disable-nls --with-password-timeout=1 \
		  --with-passprompt="[sudo] Password for %p" && \
		${PMAKE} && \
		${MAKE} install DESTDIR="${INITRD_PATH}"'

copy-sudo:
	@true

clean-sudo:
	@if [ -h source/sudo ]; then \
	  (cd source/sudo && ${QMAKE} clean); \
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
	@rm -rf "${INITRD_PATH}/lib/modules"/* && \
	(cd source/linux && ${MAKE} modules_install \
	  INSTALL_MOD_PATH="${INITRD_PATH}" &>/dev/null)


busybox: build-busybox copy-busybox

clean-busybox:
	@if [ -h source/busybox ]; then \
	  (cd source/busybox && ${MAKE} clean); \
	fi

build-busybox: build-pv
	@(cd source/busybox && \
	  cp ../../../../config/busybox/dot-config .config) && \
	./scripts/logger 'busybox' '${PLATFORM}' \
	  'cd source/busybox && ${PMAKE}'

copy-busybox:
	@echo -n "Copying files for 'busybox'... " && \
	mkdir -p "${INITRD_PATH}/bin" && \
	(cd source/busybox && \
	  cp busybox "${INITRD_PATH}/bin/busybox") && \
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
	rm -rf "${INITRD_PATH}/lib/xtables" \
	  "${INITRD_PATH}/bin/iptables-"* && \
	for type in ip4tc ip6tc ipq iptc xtables; do \
	  rm -f "${INITRD_PATH}/lib/lib$$type".so* || exit "$$?"; \
	done && \
	for type in xtables iptables ip6tables; do \
	  rm -rf "${INITRD_PATH}/sbin/$$type"*; \
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
	   ${PMAKE} && \
	   ${MAKE} install DESTDIR='${INITRD_PATH}'"

copy-iptables:
	@echo -n "Copying files for 'iptables'... " && \
	cd source/iptables && \
	rm -rf '${INITRD_PATH}/share' \
	  '${INITRD_PATH}/include' \
	  '${INITRD_PATH}/usr/lib/pkgconfig' \
	  '${INITRD_PATH}/lib/libxtables.la' && \
	echo 'done.'


xfsprogs: build-xfsprogs copy-xfsprogs

clean-xfsprogs:
	@cd source/xfsprogs && ${MAKE} clean

build-xfsprogs:
	@./scripts/logger 'xfsprogs' '${PLATFORM}' \
	  "cd source/xfsprogs && unset PLATFORM && \
	    ./configure --prefix='${INITRD_PATH}' \
	      --exec-prefix='${INITRD_PATH}' \
	      --enable-shared=yes --enable-static=no \
	      --enable-readline=no --enable-gettext=no && ${PMAKE}"

copy-xfsprogs:
	@echo -n "Copying files for 'xfsprogs'... " && \
	(cd source/xfsprogs && ${MAKE} install &>/dev/null) && \
	echo 'done.'


glibc: build-glibc copy-glibc

build-glibc:
	@./scripts/logger 'glibc' '${PLATFORM}' \
	  "cd source/glibc && unset MAKEFLAGS && \
	    mkdir -p '${STAGING_PATH}/glibc' && \
	    mkdir -p build-directory && cd build-directory && \
	    CFLAGS='-Os' ../configure --prefix='' \
	      --enable-shared --disable-static \
	      --disable-multi-arch --disable-nscd && \
	    ${PMAKE} && \
	    ${MAKE} install DESTDIR='${STAGING_PATH}/glibc'"

copy-glibc:
	@echo -n "Copying files for 'glibc'... " && \
	./scripts/install-glibc \
	  '${STAGING_PATH}/glibc' '${INITRD_PATH}' && \
	echo 'done.'

clean-glibc:
	@cd source/glibc && rm -rf build-directory


e2fsprogs: build-e2fsprogs copy-e2fsprogs

build-e2fsprogs:
	@./scripts/logger 'e2fsprogs' '${PLATFORM}' \
	  "cd source/e2fsprogs && \
	    ./configure --prefix='' --enable-shared \
	      --disable-static --enable-fsck --enable-quota \
	      --enable-elf-shlibs --enable-tls --disable-uuidd \
	      --disable-e2initrd-helper --disable-debugfs && \
	    ${PMAKE} && \
	    ${MAKE} install DESTDIR='${STAGING_PATH}/e2fsprogs'"

copy-e2fsprogs:
	@echo -n "Copying files for 'e2fsprogs'... " && \
	cd '${STAGING_PATH}/e2fsprogs' && \
	  rm -rf etc share && \
	  (cd sbin && for type in ext2 ext3 ext4 ext4dev; do \
	    rm -f "fsck.$$type" "mkfs.$$type" && \
	    ln -sf e2fsck "fsck.$$type" && ln -sf mke2fs "mkfs.$$type"; \
	  done) && \
	cp -a * '${INITRD_PATH}' && \
	echo 'done.'


stats: recalculate-stats

recalculate-stats:
	@for file in 'logs/${PLATFORM}'/*'-${TS}.log'; do \
	  stats_file="stats/${PLATFORM}/` \
	    basename "$$file" .log | sed 's/-${TS}//g;' \
	  `.count" && \
	  wc -l < "$$file" > "$$stats_file" || exit "$$?"; \
	done


# Private targets

clean-logs:
	rm -f logs/*/*

status:
	@echo && ./scripts/print-status 'start' \
	  'Building package `core` for `${PLATFORM}`'

