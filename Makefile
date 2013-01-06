
MAKE ?= make
QMAKE := ${MAKE} --no-print-directory

all: packages iso-image

compile:
	@(cd source && ${QMAKE})

packages: medic-core-pkg concierge-pkg java-pkg system-services-pkg vm-tools-pkg

clean:
	rm -f output/image.iso
	rm -rf staging/packages
	rm -f iso/packages/*.vpkg iso/boot/image.gz iso/boot/kernel

distclean: clean
	(cd source && ${MAKE} clean)
	rm -rf initrd/lib/modules/*

iso-image: initrd-image
	@echo -n 'Creating ISO image... '
	@cd iso && mkisofs -J -R -V 'Medic Mobile VM' \
		-boot-load-size 4 -boot-info-table -o ../output/image.iso \
		-no-emul-boot -b boot/isolinux/isolinux.bin \
		-c boot/isolinux/boot.cat . &>/dev/null
	@echo 'done.'

initrd-image:
	@echo -n 'Creating initrd image... '
	@cd initrd && \
		find * | cpio -o -H newc 2>/dev/null | gzip -c9 \
			> ../iso/boot/image.gz
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

convert-boot-logo:
	pngtopnm kernel/boot-logo/logo_medic_clut224.png | ppmquant 224 \
		| pnmtoplainpnm > kernel/boot-logo/logo_medic_clut224.ppm

