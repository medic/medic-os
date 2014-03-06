
MAKE ?= make
PLATFORM ?= x86
QMAKE := ${MAKE} --no-print-directory

MEDIC_CORE_VERSION := 1.5.0
MEDIC_CORE_ROOT := /srv/software/medic-core/v${MEDIC_CORE_VERSION}/${PLATFORM}

all: packages build-iso build-xen-image compress-xen-image build-x86-image-nopae

iso: build-iso

xen: build-xen-image

compile:
	@(cd source && ${QMAKE})

copy:
	@(cd source && ${QMAKE} copy)

compiler:
	@(cd source && ${QMAKE} compiler)


packages: strip-binaries medic-core-pkg concierge-pkg java-pkg system-services-pkg vm-tools-pkg gardener-pkg kujua-transport-pkg

clean:
	rm -rf output/* && \
	rm -rf staging/packages && \
	rm -rf packages/vm-tools/software && \
	rm -rf packages/medic-core/software && \
	rm -rf "images/${PLATFORM}/iso/packages"/*

distclean: clean
	for platform in x86 x64 armv6; do \
	  rm -rf "initrd/$$platform/lib/modules/"* && \
	  for type in iso xen; do \
	    rm -f "images/$$platform/$$type/boot/kernel" \
	      "images/$$platform/$$type/boot/image.xz" \
		  "images/$$platform/$$type/packages"/*; \
	  done; \
	done && \
	(cd source && ${MAKE} clean)

clean-compiler:
	(cd source && ${MAKE} clean-compiler)


build-iso: verify-packages build-initrd
	@echo -n 'Creating ISO image... ' && \
	cd "images/${PLATFORM}/iso" && mkisofs -J -R \
		-V 'Medic Mobile VM' \
		-o "../../../output/image-${PLATFORM}${ISO_EXTRA}.iso" \
		-boot-load-size 4 -boot-info-table \
		-no-emul-boot -b boot/isolinux/isolinux.bin \
		-c boot/isolinux/boot.cat . &>/dev/null && \
	echo 'done.'

build-xen-image:
	@echo -n 'Creating Xen image... ' && \
	\
	loop_path="staging/loopback" && \
	image_path="output/image-${PLATFORM}-xen" && \
	\
	dd if=/dev/zero of="$$image_path" \
		bs=1048576 seek=1023 count=1 &>/dev/null && \
	\
	mkfs.ext4 -F "$$image_path" &>/dev/null && \
	mount -o loop "$$image_path" "$$loop_path" && \
	cp -a "images/${PLATFORM}/xen"/* "$$loop_path" && \
	mkdir -p "$$loop_path/packages" && \
	\
	cp -a "images/${PLATFORM}/iso/packages"/* \
		"$$loop_path/packages" && \
	\
	sync && umount "$$loop_path" && sync && \
	echo 'done.'

build-ami-image:
	@echo -n 'Building AMI instance-store image... ' && \
	\
	source "config/aws/settings" && \
	export PATH="$$EC2_HOME/bin:$$PATH" && \
	\
	case '${PLATFORM}' in \
	  x64) architecture='x86_64';; \
	  x86) architecture='i386';; \
	  *) echo 'Fatal: Unsupported platform' >&2; exit 1 ;; \
	esac && \
	\
	rm -rf "output/image-${PLATFORM}-ami" && \
	mkdir -p "output/image-${PLATFORM}-ami" && \
	ln -f "output/image-${PLATFORM}-xen" "output/image" && \
	\
	ec2-bundle-image \
		-c "$$EC2_CERTIFICATE" -k "$$EC2_PRIVATE_KEY" \
		-u "$$AWS_ID" -d "output/image-${PLATFORM}-ami" \
		-i "output/image" -r "$$architecture" >/dev/null || exit "$$?"; \
	\
	rm -f "output/image"; \
	echo 'done.'

compress-xen-image:
	@gzip -qcf9 "output/image-${PLATFORM}-xen" \
		> "output/image-${PLATFORM}-xen.gz"

upload: upload-ami-image

upload-ami-image: build-ami-image
	@echo -n 'Uploading AMI instance-store image... ' && \
	\
	source "config/aws/settings" && \
	export PATH="$$EC2_HOME/bin:$$PATH" && \
	\
	ec2-upload-bundle \
		-a "$$AWS_ACCESS_KEY" \
		-s "$$AWS_SECRET_KEY" -b "$$S3_BUCKET" \
		-m "output/image-${PLATFORM}-ami/image.manifest.xml" \
			|| exit "$$?"; \
	echo 'done.'

build-initrd:
	@echo -n 'Creating initrd image... ' && \
	cp -a initrd/common/* "initrd/${PLATFORM}/" && \
	(cd "initrd/${PLATFORM}" && \
		find * | cpio -o -H newc 2>/dev/null \
		  | sh ../../source/core/source/linux/scripts/xz_wrap.sh \
			> "../../images/${PLATFORM}/iso/boot/image.xz") && \
	(cd "images/${PLATFORM}" && \
		[ -d xen ] && cp -a "iso/boot/image.xz" "xen/boot/") && \
	echo 'done.'

build-x86-image-nopae:
	@if [ "${PLATFORM}" = 'x86' ]; then \
		echo 'Building x86 image with PAE disabled...' && \
		(cd source && \
		  ${QMAKE} rebuild-kernel KERNEL_EXTRA='-nopae' && \
		  ${QMAKE} rebuild-vm-tools-modules) && \
		\
		${QMAKE} build-iso ISO_EXTRA='-nopae'; \
	fi

strip-binaries:
	@echo -n "Removing unnecessary symbols... " && \
	./scripts/strip-binaries packages && \
	echo 'done.'

verify-packages:
	@echo -n "Verifying package contents... " && \
	./scripts/verify-packages "${MEDIC_CORE_ROOT}" && \
	echo 'done.'

concierge-pkg:
	@echo -n "Compressing package 'concierge'... " && \
	scripts/build-package 'concierge' 1000 "${PLATFORM}" && \
	echo 'done.'

java-pkg:
	@echo -n "Compressing package 'java'... " && \
	scripts/build-package 'java' 1745 "${PLATFORM}" && \
	echo 'done.'

medic-core-pkg:
	@echo -n "Compressing package 'medic-core'... " && \
	scripts/build-package 'medic-core' 1400 "${PLATFORM}" && \
	echo 'done.'

system-services-pkg:
	@echo -n "Compressing package 'system-services'... " && \
	scripts/build-package 'system-services' 1000 "${PLATFORM}" && \
	echo 'done.'

vm-tools-pkg:
	@echo -n "Compressing package 'vm-tools'... " && \
	scripts/build-package 'vm-tools' 9400 "${PLATFORM}" && \
	echo 'done.'

shrink-gardener:
	@./scripts/shrink-gardener

gardener-pkg: shrink-gardener
	@echo -n "Compressing package 'gardener'... " && \
	scripts/build-package 'gardener' 1003 "${PLATFORM}" && \
	echo 'done.'

kujua-transport-pkg:
	@echo -n "Compressing package 'kujua-transport'... " && \
	scripts/build-package 'kujua-transport' 1000 "${PLATFORM}" && \
	echo 'done.'

convert-boot-logo:
	for file in logo-medic logo-medic-gray; do \
		pngtopnm "config/kernel/common/boot-logo/$$file.png" \
		  | ppmquant 224 2>/dev/null | pnmtoplainpnm \
			> "config/kernel/common/boot-logo/$$file.ppm"; \
	done

download:
	@rm -f status/download.finished && \
	./scripts/retrieve-sources core source/core/incoming && \
	  (cd source/core && ./scripts/rearrange-sources) && \
	./scripts/retrieve-sources compiler source/compiler/incoming && \
	  (cd source/compiler && ./scripts/rearrange-sources) && \
	./scripts/retrieve-sources medic-core source/medic-core/incoming && \
	  (cd source/medic-core && ./scripts/rearrange-sources) && \
	./scripts/retrieve-sources vm-tools source/vm-tools/incoming && \
	  (cd source/vm-tools && ./scripts/rearrange-sources) && \
	touch status/download.finished

move-downloaded:
	@if ! [ -f status/move.finished ]; then \
	  (cd source/core && mv incoming/* source) && \
	  (cd source/compiler && mv incoming/* source) && \
	  (cd source/vm-tools && mv incoming/* source) && \
	  (cd source/medic-core && mv incoming/* source) && \
	  touch status/move.finished; \
  fi

delete-downloaded:
	@rm -f status/* && \
	for type in incoming source; do \
	  for pkg in core compiler vm-tools medic-core; do \
		(cd source && rm -rf "$$pkg/$$type"/*); \
		done; \
	done

