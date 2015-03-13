
# Variables

MAKE ?= make
QMAKE := ${MAKE} --no-print-directory
PLATFORM := $(shell ./scripts/detect-platform)

DATE := $(shell date +'%Y%m%d')
TIMESTAMP := $(shell date +'%Y-%m-%d %H:%M:%S')

SYSTEM_ID ?= medic-os
SYSTEM_NAME ?= Medic OS
SYSTEM_VERSION_STRING ?= 1.1.0
SYSTEM_IMAGE_ID := ${SYSTEM_ID}-v${SYSTEM_VERSION_STRING}-${PLATFORM}

COMPILER_VERSION ?= 4.8.2
COMPILER_ROOT ?= /srv/software/compiler/v${COMPILER_VERSION}/${PLATFORM}


# Meta-targets

all: packages images timestamp-output

iso: build-iso-image
xen: build-xen-image
native: build-native-tarball

images: build-iso-image build-xen-image compress-xen-image build-native-tarball build-x86-image-nopae
packages: strip-binaries medic-core-pkg concierge-pkg java-pkg system-services-pkg vm-tools-pkg gardener-pkg medic-transport-pkg


# Public targets

compile:
	@if [ -z '${SKIP_COMPILER}' ]; then \
	  ${QMAKE} compiler || exit 1; \
	fi
	@(cd source && ../scripts/prepend-path \
	  '${COMPILER_ROOT}' ${QMAKE} PLATFORM='${PLATFORM}')

copy:
	@(cd source && \
	  ${QMAKE} copy PLATFORM='${PLATFORM}')

compiler:
	@(cd source && ${QMAKE} compiler \
	  PLATFORM='${PLATFORM}' COMPILER_ROOT='${COMPILER_ROOT}')

clean:
	rm -rf output/* && \
	rm -rf staging/tarball

distclean: clean clean-source clean-staging
	for platform in x86 x64 armv6; do \
	  rm -rf "images/$$platform/iso/packages"/* && \
	  rm -rf "initrd/$$platform/lib/modules"/* && \
	  for type in iso xen; do \
	    rm -f "images/$$platform/$$type/boot/kernel" \
	      "images/$$platform/$$type/boot/image.xz" \
	        "images/$$platform/$$type/packages"/*; \
	  done; \
	done

clean-compiler:
	(cd source && ${MAKE} clean-compiler)


# Private targets

clean-source:
	(cd source && ${MAKE} clean)

clean-staging:
	rm -rf staging/packages && \
	rm -rf packages/vm-tools/software && \
	rm -rf packages/medic-core/software && \
	rm -rf 'staging/software/${PLATFORM}'/*

build-iso-image: build-initrd
	@echo -n 'Creating ISO image... ' && \
	\
	cd 'images/${PLATFORM}/iso' && mkisofs -J -R \
	  -V '${SYSTEM_NAME} v${SYSTEM_VERSION_STRING}' \
	  -o '../../../output/${SYSTEM_IMAGE_ID}${ISO_EXTRA}-virtual.iso' \
	  -boot-load-size 4 -boot-info-table \
	  -no-emul-boot -b boot/isolinux/isolinux.bin \
	  -c boot/isolinux/boot.cat . &>/dev/null && \
	\
	echo 'done.'

build-xen-image:
	@echo -n 'Creating Xen image... ' && \
	loop_path="staging/loopback" && \
	image_path='output/${SYSTEM_IMAGE_ID}-xen' && \
	ext4_options='none,extent,large_file,has_journal' && \
	\
	dd if=/dev/zero of="$$image_path" \
	  bs=1048576 seek=1023 count=1 &>/dev/null && \
	\
	mkfs.ext4 -O "$$ext4_options" -qF "$$image_path" >/dev/null && \
	mount -o loop "$$image_path" "$$loop_path" && \
	cp -a 'images/${PLATFORM}/xen'/* "$$loop_path/" && \
	mkdir -p "$$loop_path/packages" && \
	\
	cp -a 'images/${PLATFORM}/iso/packages'/* \
	  "$$loop_path/packages/" && \
	\
	sync && umount "$$loop_path" && sync && \
	echo 'done.'

build-ami-image:
	@echo -n 'Building AMI instance-store image... ' && \
	source 'config/aws/settings' && \
	export PATH="$$EC2_HOME/bin:$$PATH" && \
	\
	case '${PLATFORM}' in \
	  x64) architecture='x86_64';; \
	  x86) architecture='i386';; \
	  *) echo 'Fatal: Unsupported platform' >&2; exit 1 ;; \
	esac && \
	\
	rm -rf 'output/${SYSTEM_IMAGE_ID}-ami' && \
	mkdir -p 'output/${SYSTEM_IMAGE_ID}-ami' && \
	ln -f 'output/${SYSTEM_IMAGE_ID}-xen' 'output/image' && \
	\
	ec2-bundle-image \
	  -c "$$EC2_CERTIFICATE" -k "$$EC2_PRIVATE_KEY" \
	  -u "$$AWS_ID" -d 'output/${SYSTEM_IMAGE_ID}-ami' \
	  -i 'output/image' -r "$$architecture" >/dev/null || exit "$$?"; \
	\
	rm -f output/image; \
	echo 'done.'

build-native-tarball:
	@echo -n 'Creating native tarball... ' && \
	staging_dir='staging/tarball/${PLATFORM}/${SYSTEM_IMAGE_ID}-native' && \
	\
	mkdir -p "$$staging_dir" && \
	cp -a 'initrd/${PLATFORM}'/* "$$staging_dir/" && \
	(cd "$$staging_dir" && mkdir -p srv mnt/startup) && \
	\
	cp -a 'images/${PLATFORM}/iso/packages' \
	  "$$staging_dir/mnt/startup/" && \
	\
	(cd "$$staging_dir/../" && tar -cf - .) \
	  | xz -9ec > "output/${SYSTEM_IMAGE_ID}-native.tar.xz" && \
	\
	echo 'done.'

compress-xen-image:
	@echo -n 'Compressing Xen image... ' && \
	\
	xz -0qcf 'output/${SYSTEM_IMAGE_ID}-xen' \
	  > 'output/${SYSTEM_IMAGE_ID}-xen.xz' && \
	\
	rm -f 'output/${SYSTEM_IMAGE_ID}-xen' && \
	echo 'done.'

upload: upload-ami-image

upload-ami-image: build-ami-image
	@echo -n 'Uploading AMI instance-store image... ' && \
	source 'config/aws/settings' && \
	export PATH="$$EC2_HOME/bin:$$PATH" && \
	\
	ec2-upload-bundle \
	  -a "$$AWS_ACCESS_KEY" \
	  -s "$$AWS_SECRET_KEY" -b "$$S3_BUCKET" \
	  -m 'output/${SYSTEM_IMAGE_ID}-ami/image.manifest.xml' \
	    || exit "$$?"; \
	\
	echo 'done.'

copy-compiler-libraries:
	@echo -n 'Copying compiler libraries...' && \
	export PATH="${COMPILER_ROOT}/bin:$$PATH" && \
	active_compiler_root="$$(dirname "$$(which gcc)")/../" && \
	\
	find "$$active_compiler_root"/lib* \
	  \( -type f -o -type l \) -a \( \
	     -name 'libstdc++.so*' -o \
		 -name 'libssp.so*' -o -name 'libgcc_s.so*' \
	  \) -exec cp -a {} 'initrd/${PLATFORM}/lib/' \; && \
	\
	for file in \
	  'initrd/${PLATFORM}/lib/libssp.so'* \
	  'initrd/${PLATFORM}/lib/libgcc_s.so'*; do \
	  \
	  chmod +x "$$file" && \
	  strip --strip-unneeded "$$file" || exit "$$?"; \
	done && \
	\
	echo 'done.'

build-initrd: copy-compiler-libraries
	@echo -n 'Creating initrd image... ' && \
	chmod 0440 initrd/common/etc/sudoers && \
	cp -a initrd/common/* 'initrd/${PLATFORM}/' && \
	\
	echo '${TIMESTAMP}' \
	  > 'initrd/${PLATFORM}/etc/build-timestamp' && \
	\
	(cd 'initrd/${PLATFORM}' && \
	  find * | cpio -o -H newc 2>/dev/null \
	    | sh ../../source/core/source/linux/scripts/xz_wrap.sh \
	      > '../../images/${PLATFORM}/iso/boot/image.xz') && \
	\
	(cd 'images/${PLATFORM}' && \
	  [ -d xen ] && cp -a 'iso/boot/image.xz' 'xen/boot/') && \
	\
	echo 'done.'

build-x86-image-nopae:
	@if [ '${PLATFORM}' = 'x86' ]; then \
	  echo 'Building x86 image with PAE disabled...' && \
	  (cd source && \
	    ${QMAKE} rebuild-kernel KERNEL_EXTRA='-nopae' && \
	    ${QMAKE} rebuild-vm-tools-modules) && \
	  \
	  ${QMAKE} build-iso-image ISO_EXTRA='-nopae'; \
	fi

strip-binaries:
	@echo -n 'Removing unnecessary symbols... ' && \
	./scripts/strip-binaries packages && \
	echo 'done.'

concierge-pkg:
	@echo -n 'Compressing package 'concierge'... ' && \
	scripts/build-package 'concierge' 10100 '${PLATFORM}' && \
	echo 'done.'

java-pkg:
	@echo -n 'Compressing package 'java'... ' && \
	scripts/build-package 'java' 17650 '${PLATFORM}' && \
	echo 'done.'

medic-core-pkg:
	@echo -n 'Compressing package 'medic-core'... ' && \
	scripts/build-package 'medic-core' 16000 '${PLATFORM}' && \
	echo 'done.'

system-services-pkg:
	@echo -n 'Compressing package 'system-services'... ' && \
	scripts/build-package 'system-services' 10100 '${PLATFORM}' && \
	echo 'done.'

vm-tools-pkg:
	@echo -n 'Compressing package 'vm-tools'... ' && \
	scripts/build-package 'vm-tools' 94010 '${PLATFORM}' && \
	echo 'done.'

shrink-gardener:
	@./scripts/shrink-gardener

gardener-pkg: shrink-gardener
	@echo -n 'Compressing package 'gardener'... ' && \
	scripts/build-package 'gardener' 11000 '${PLATFORM}' && \
	echo 'done.'

medic-transport-pkg:
	@echo -n 'Compressing package 'medic-transport'... ' && \
	scripts/build-package 'medic-transport' 10100 '${PLATFORM}' && \
	echo 'done.'

convert-boot-logo:
	for file in logo-medic logo-medic-gray; do \
	  pngtopnm "config/kernel/common/boot-logo/$$file.png" \
	    | ppmquant 224 2>/dev/null | pnmtoplainpnm \
	      > "config/kernel/common/boot-logo/$$file.ppm"; \
	done

download:
	@rm -f status/download.finished && \
	./scripts/retrieve-sources \
	  core source/core/incoming '${URL}' && \
	    (cd source/core && ./scripts/rearrange-sources) && \
	./scripts/retrieve-sources \
	  compiler source/compiler/incoming '${URL}' && \
	    (cd source/compiler && ./scripts/rearrange-sources) && \
	./scripts/retrieve-sources \
	  medic-core source/medic-core/incoming '${URL}' && \
	    (cd source/medic-core && ./scripts/rearrange-sources) && \
	./scripts/retrieve-sources \
	  vm-tools source/vm-tools/incoming '${URL}' && \
	    (cd source/vm-tools && ./scripts/rearrange-sources) && \
	touch status/download.finished

move-downloaded:
	@if ! [ -f status/move.finished ]; then \
	  (cd source/core && mv incoming/* source/) && \
	  (cd source/compiler && mv incoming/* source/) && \
	  (cd source/vm-tools && mv incoming/* source/) && \
	  (cd source/medic-core && mv incoming/* source/) && \
	  touch status/move.finished; \
  fi

delete-downloaded:
	@rm -f status/* && \
	for type in incoming source; do \
	  for pkg in core compiler vm-tools medic-core; do \
	    (cd source && rm -rf "$$pkg/$$type"/*); \
	  done; \
	done

timestamp-output:
	@for path in output/medic-os-*; do \
	  target="`echo "$$path" | sed 's/\(\.[a-z\.]\+\)$$/-${DATE}\1/;'`" && \
	  mv "$$path" "$$target"; \
	done

