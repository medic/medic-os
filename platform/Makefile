
# Variables

MAKE ?= make
SHELL := /bin/bash
QMAKE := ${MAKE} --no-print-directory

PLATFORM := $(shell ./scripts/detect-platform)

DIR := initrd/${PLATFORM}
INITRD_PATH := $(shell readlink -f '${DIR}' 2>/dev/null || realpath '${DIR}')

DATE := $(shell date +'%Y%m%d')
TIMESTAMP := $(shell date +'%Y-%m-%d %H:%M:%S')

SYSTEM_ID ?= medic-os
SYSTEM_NAME ?= Medic OS
SYSTEM_VERSION_STRING ?= 1.1.0
SYSTEM_IMAGE_NAME := ${SYSTEM_ID}-v${SYSTEM_VERSION_STRING}

COMPILER_VERSION ?= 4.8.2
COMPILER_ROOT ?= /srv/software/compiler/v${COMPILER_VERSION}/${PLATFORM}

ifeq (${RELEASE},)
  SYSTEM_IMAGE_ID := ${SYSTEM_IMAGE_NAME}-${DATE}-${PLATFORM}
else
  SYSTEM_IMAGE_ID := ${SYSTEM_IMAGE_NAME}-${PLATFORM}
endif


# Meta-targets

all: bootinit packages images

initrd: build-initrd
native: build-native-tarball
docker: build-docker-staging

images: build-native-tarball build-docker-staging
packages: strip-binaries medic-core-pkg medic-rdbms-pkg medic-couch2pg-pkg horticulturalist-pkg medic-api-pkg medic-sentinel-pkg system-services-pkg vm-tools-pkg


# Public targets

bootinit:
	@(cd initrd/common && tar -cf - boot | xz -c) \
	  > ../output/"${SYSTEM_IMAGE_NAME}-${DATE}-all-bootinit.tar.xz"

compile-only:
	@(cd source && ../scripts/prepend-path '${COMPILER_ROOT}' \
	  ${QMAKE} build PLATFORM='${PLATFORM}' COMPILER_ROOT='${COMPILER_ROOT}')

build: build-start package-docs build-finish

copy:
	@(cd source && \
	  ${QMAKE} copy PLATFORM='${PLATFORM}')

compiler:
	@(cd source && ${QMAKE} compiler \
	  PLATFORM='${PLATFORM}' COMPILER_ROOT='${COMPILER_ROOT}')

clean:
	rm -rf output/* && \
	rm -rf staging/tarball staging/docker

distclean: clean clean-images clean-staging clean-source

clean-compiler:
	(cd source && ${MAKE} clean-compiler)


# Private targets

package-docs: medic-core-docs-pkg medic-rdbms-docs-pkg vm-tools-docs-pkg

build-start:
	@(cd source && ../scripts/prepend-path '${COMPILER_ROOT}' \
	  ${QMAKE} build \
	    PLATFORM='${PLATFORM}' COMPILER_ROOT='${COMPILER_ROOT}')

build-finish:
	@(cd source && ../scripts/prepend-path '${COMPILER_ROOT}' \
	  ${QMAKE} copy finalize \
	    PLATFORM='${PLATFORM}' COMPILER_ROOT='${COMPILER_ROOT}')

clean-source:
	(cd source && ${MAKE} clean)

clean-staging:
	rm -rf staging/packages && \
	rm -rf packages/vm-tools/software && \
	rm -rf packages/medic-core/software && \
	rm -rf packages/medic-rdbms/software && \
	rm -rf 'staging/software/${PLATFORM}'/*

clean-images:
	for platform in x86 x64 armv6 noarch; do \
	  rm -rf "images/$$platform/iso/packages"/* && \
	  rm -rf "initrd/$$platform/lib/modules"/* && \
	  for type in iso; do \
	    rm -f "images/$$platform/$$type/boot/kernel" \
	      "images/$$platform/$$type/boot/image.xz" \
	        "images/$$platform/$$type/packages"/* || exit "$$?"; \
	  done || exit "$$?"; \
	done

build-docker-staging:
	@echo -n 'Staging files for Docker... ' && \
	\
	base_dir='staging/docker/${PLATFORM}' && \
	staging_dir="$$base_dir/${SYSTEM_IMAGE_ID}-docker" && \
	\
	mkdir -p "$$staging_dir" && \
	cp -a initrd/common/boot "$$staging_dir/" && \
	(cd "$$staging_dir" && mkdir -p srv mnt/startup) && \
	\
	cp -a 'images/${PLATFORM}/iso/packages' \
	  "$$staging_dir/mnt/startup/" && \
	\
	echo 'done.'

build-native-tarball:
	@echo -n 'Creating native tarball... ' && \
	\
	base_dir='staging/tarball/${PLATFORM}' && \
	staging_dir="$$base_dir/${SYSTEM_IMAGE_ID}-native" && \
	\
	mkdir -p "$$staging_dir" && \
	cp -a '${INITRD_PATH}'/* "$$staging_dir/" && \
	(cd "$$staging_dir" && mkdir -p srv mnt/startup) && \
	\
	cp -a 'images/${PLATFORM}/iso/packages' \
	  "$$staging_dir/mnt/startup/" && \
	\
	(cd "$$staging_dir/../" && tar -cf - .) \
	  | xz -c > "output/${SYSTEM_IMAGE_ID}-native.tar.xz" && \
	\
	echo 'done.'

check-compiler-root:
	@if [ '${BUILD_COMPILER}' -a -z '${COMPILER_ROOT}' ]; then \
	  echo 'Warning: Compiler root not specified' >&2; \
	  echo 'Warning: Resulting image may be incomplete' >&2; \
	fi

copy-compiler-libraries: check-compiler-root
	@echo -n 'Copying compiler libraries...' && \
	\
	if [ '${BUILD_COMPILER}' ]; then \
	  export PATH="${COMPILER_ROOT}/bin:$$PATH"; \
	fi && \
	\
	mkdir -p '${INITRD_PATH}/lib' && \
	compiler_root="`dirname "$$(gcc --print-libgcc-file-name)"`" && \
	\
	cd "$$compiler_root" && \
	\
	for lib in gcc_s stdc++; do \
	  cp -aL "lib$$lib.so"* '${INITRD_PATH}/lib' && \
	  chmod +x "${INITRD_PATH}/lib/lib$$lib.so"* \
            || exit "$$?"; \
	done && \
	\
	echo 'done.'

prepare-initrd:
	@echo -n 'Preparing initrd files... ' && \
	\
	./scripts/strip-binaries '${INITRD_PATH}' && \
	cd "${INITRD_PATH}" && \
	\
	find . -name '*.la' \
	  -type f -exec rm -f {} \; && \
	\
	find . \( -name '*.so' -o -name '*.so.*' \) \
	  -type f -exec chmod +x {} \; && \
	\
	rm -rf include share lib/*.py && \
	echo 'done.'

build-initrd: copy-compiler-libraries prepare-initrd
	@echo -n 'Creating initrd image... ' && \
	\
	chmod 0440 initrd/common/etc/sudoers && \
	cp -a initrd/common/* '${INITRD_PATH}/' && \
	\
	echo '${TIMESTAMP}' \
	  > '${INITRD_PATH}/etc/build-timestamp'

strip-binaries:
	@echo -n 'Removing unnecessary symbols... ' && \
	./scripts/strip-binaries packages && \
	echo 'done.'

horticulturalist-pkg:
	@echo -n 'Compressing package 'horticulturalist'... ' && \
	scripts/build-package 'horticulturalist' 00910 '${PLATFORM}' && \
	echo 'done.'

medic-api-pkg:
	@echo -n 'Compressing package 'medic-api'... ' && \
	scripts/build-package 'medic-api' 00101 '${PLATFORM}' && \
	echo 'done.'

medic-core-pkg:
	@echo -n 'Compressing package 'medic-core'... ' && \
	scripts/build-package 'medic-core' 21101 '${PLATFORM}' && \
	echo 'done.'

medic-core-docs-pkg:
	@echo -n 'Compressing package 'medic-core-docs'... ' && \
	scripts/build-docs-package \
	  'medic-core-docs' 21101 /srv/software/medic-core && \
	echo 'done.'

medic-couch2pg-pkg:
	@echo -n 'Compressing package 'medic-couch2pg'... ' && \
	scripts/build-package 'medic-couch2pg' 10000 '${PLATFORM}' && \
	echo 'done.'

medic-rdbms-pkg:
	@echo -n 'Compressing package 'medic-rdbms'... ' && \
	scripts/build-package 'medic-rdbms' 95301 '${PLATFORM}' && \
	echo 'done.'

medic-rdbms-docs-pkg:
	@echo -n 'Compressing package 'medic-rdbms-docs'... ' && \
	scripts/build-docs-package \
	  'medic-rdbms-docs' 95301 /srv/software/medic-rdbms && \
	echo 'done.'

medic-sentinel-pkg:
	@echo -n 'Compressing package 'medic-sentinel'... ' && \
	scripts/build-package 'medic-sentinel' 00101 '${PLATFORM}' && \
	echo 'done.'

system-services-pkg:
	@echo -n 'Compressing package 'system-services'... ' && \
	scripts/build-package 'system-services' 10300 '${PLATFORM}' && \
	echo 'done.'

vm-tools-pkg:
	@echo -n 'Compressing package 'vm-tools'... ' && \
	scripts/build-package 'vm-tools' 94612 '${PLATFORM}' && \
	echo 'done.'

vm-tools-docs-pkg:
	@echo -n 'Compressing package 'vm-tools-docs'... ' && \
	scripts/build-docs-package \
	  'vm-tools-docs' 94613 /srv/software/vm-tools && \
	echo 'done.'

convert-boot-logo:
	for file in logo-medic logo-medic-gray; do \
	  pngtopnm "config/kernel/common/boot-logo/$$file.png" \
	    | ppmquant 224 2>/dev/null | pnmtoplainpnm \
	      > "config/kernel/common/boot-logo/$$file.ppm" || exit "$$?"; \
	done

download:
	@rm -f status/download.finished && \
	for manifest in source/manifests/*; do \
	  pkg="`basename "$$manifest"`" && \
	  if [ -f "$$manifest" -a "$$pkg" != '*' ]; then \
	    ./scripts/retrieve-sources \
	      "$$pkg" "source/$$pkg/incoming" '${URL}' && \
	    (cd "source/$$pkg" && ./scripts/rearrange-sources) || exit "$$?"; \
	  fi; \
	done && \
	touch status/download.finished;

move-downloaded:
	@if ! [ -f status/move.finished ]; then \
	  for manifest in source/manifests/*; do \
	    pkg="`basename "$$manifest"`" && \
	    if [ -f "$$manifest" -a "$$pkg" != '*' ]; then \
	      (cd "source/$$pkg" && find incoming -mindepth 1 \
	        -maxdepth 1 -exec mv {} source/ \;) || exit "$$?"; \
	    fi; \
	  done && \
	  touch status/move.finished; \
	fi

delete-downloaded:
	@rm -f status/* && \
	for type in incoming source; do \
	  for manifest in source/manifests/*; do \
	    pkg="`basename "$$manifest"`" && \
	    if [ -f "$$manifest" -a "$$pkg" != '*' ]; then \
	      (cd "source/$$pkg" && rm -rf "$$type"/[^.]*) || exit "$$?"; \
	    fi; \
	  done; \
	done

