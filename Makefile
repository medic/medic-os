
MAKE ?= make
QMAKE := ${MAKE} --no-print-directory

all: packages iso-image

compile:
	@(cd source && ${QMAKE})

packages: medic-core-pkg concierge-pkg java-pkg system-services-pkg

clean:
	rm -f output/image.iso

distclean: clean
	(cd source && ${MAKE} clean)
	rm -f iso/packages/*.tar.xz iso/boot/image.gz

iso-image: initrd-image
	@echo -n 'Creating ISO image... '
	@cd iso && mkisofs -J -R -V 'Medic Mobile Virtual Appliance' \
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
	@cd packages/concierge && \
		tar -cf - * | xz -9ec > ../../iso/packages/concierge-v1000-x86.tar.xz
	@echo 'done.'

java-pkg:
	@echo -n "Compressing package 'java'... "
	@cd packages/java && \
		tar -cf - * | xz -9ec > ../../iso/packages/java-v1790-x86.tar.xz
	@echo 'done.'

medic-core-pkg:
	@echo -n "Compressing package 'medic-core'... "
	@cd packages/medic-core && \
		tar -cf - * | xz -9ec > ../../iso/packages/medic-core-v1200-x86.tar.xz
	@echo 'done.'

system-services-pkg:
	@echo -n "Compressing package 'system-services'... "
	@cd packages/system-services && \
		tar -cf - * | xz -9ec > ../../iso/packages/system-services-v1000-x86.tar.xz
	@echo 'done.'

