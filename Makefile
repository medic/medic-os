
all: iso-image

clean:
	rm -f output/image.iso

iso-image: initrd-image medic-core-pkg java-pkg concierge-pkg system-pkg
	cd iso && mkisofs -J -R -V 'VM Startup Disk' \
		-boot-load-size 4 -boot-info-table \
		-o ../output/image.iso -no-emul-boot \
		-b boot/isolinux/isolinux.bin -c boot/isolinux/boot.cat .

initrd-image:
	cd initrd && \
		find * | cpio -o -H newc | gzip -c9 > ../iso/boot/image.gz

concierge-pkg:
	cd packages/concierge && \
		tar -cJf ../../iso/packages/concierge-v1000-x86.tar.xz *

java-pkg:
	cd packages/java && \
		tar -cJf ../../iso/packages/java-v1790-x86.tar.xz *

medic-core-pkg:
	cd packages/medic-core && \
		tar -cJf ../../iso/packages/medic-core-v1200-x86.tar.xz *

system-pkg:
	cd packages/system && \
		tar -cJf ../../iso/packages/system-v1000-x86.tar.xz *

