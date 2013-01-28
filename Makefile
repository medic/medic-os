
MAKE ?= make
QMAKE := ${MAKE} --no-print-directory


all: packages iso-image

compile:
	@(cd source && ${QMAKE})

copy:
	@(cd source && ${QMAKE} copy)

packages: strip-binaries medic-core-pkg concierge-pkg java-pkg system-services-pkg vm-tools-pkg gardener-pkg

clean:
	rm -f output/image.iso
	rm -rf staging/packages

distclean: clean
	rm -rf initrd/lib/modules/*
	rm -f iso/boot/kernel iso/boot/image.gz iso/packages/*
	(cd source && ${MAKE} clean)

clean-iso:
	rm -f iso/packages/*.vpkg iso/boot/image.gz iso/boot/kernel

iso-image: initrd-image verify-packages
	@echo -n 'Creating ISO image... '
	@cd iso && mkisofs -J -R -V 'Medic Mobile VM' \
		-boot-load-size 4 -boot-info-table -o ../output/image.iso \
		-no-emul-boot -b boot/isolinux/isolinux.bin \
		-c boot/isolinux/boot.cat . &>/dev/null
	@echo 'done.'

initrd-image:
	@echo -n 'Creating initrd image... '
	@cd initrd && \
		find * | cpio -o -H newc 2>/dev/null | xz -9ec \
			> ../iso/boot/image.xz
	@echo 'done.'

strip-binaries:
	@echo -n "Removing unnecessary symbols... "
	@./scripts/strip-binaries packages
	@echo 'done.'

verify-packages:
	@echo -n "Verifying package contents... "
	@./scripts/verify-packages
	@echo 'done.'

concierge-pkg:
	@echo -n "Compressing package 'concierge'... "
	@scripts/build-package 'concierge' 1000
	@echo 'done.'

java-pkg:
	@echo -n "Compressing package 'java'... "
	@scripts/build-package 'java' 1790
	@echo 'done.'

medic-core-pkg:
	@echo -n "Compressing package 'medic-core'... "
	@scripts/build-package 'medic-core' 1200
	@echo 'done.'

system-services-pkg:
	@echo -n "Compressing package 'system-services'... "
	@scripts/build-package 'system-services' 1000
	@echo 'done.'

vm-tools-pkg:
	@echo -n "Compressing package 'vm-tools'... "
	@scripts/build-package 'vm-tools' 9200
	@echo 'done.'

shrink-gardener:
	@./scripts/shrink-gardener

gardener-pkg: shrink-gardener
	@echo -n "Compressing package 'gardener'... "
	@scripts/build-package 'gardener' 1000
	@echo 'done.'

convert-boot-logo:
	for file in logo-medic logo-medic-gray; do \
		pngtopnm "kernel/boot-logo/$$file.png" | ppmquant 224 2>/dev/null \
			| pnmtoplainpnm > "kernel/boot-logo/$$file.ppm"; \
	done

