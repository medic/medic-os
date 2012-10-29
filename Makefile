
all: iso-image

clean:
	rm -f output/image.iso

iso-image: initrd-image medic-core-pkg java-pkg
	cd iso && mkisofs -J -R -V 'VM Startup Disk' \
		-boot-load-size 4 -boot-info-table \
		-o ../output/image.iso -no-emul-boot \
		-b boot/isolinux/isolinux.bin -c boot/isolinux/boot.cat .

initrd-image:
	cd initrd && \
		find * | cpio -o -H newc | gzip -c9 > ../iso/boot/image.gz

java-pkg:
	cd packages/java && \
		tar -cJf ../../iso/packages/java-v1790-x86.tar.xz *

medic-core-pkg:
	cd packages/medic-core && \
		tar -cJf ../../iso/packages/medic-core-v1200-x86.tar.xz *

