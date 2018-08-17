
# Variables

MAKE ?= make
SHELL := /bin/bash
QMAKE := ${MAKE} --no-print-directory

PLATFORM := $(shell ./scripts/detect-platform)

DATE := $(shell date +'%Y%m%d')
TIMESTAMP := $(shell date +'%Y-%m-%d %H:%M:%S')

SYSTEM_ID ?= medic-os
SYSTEM_NAME ?= Medic OS
SYSTEM_VERSION_STRING ?= 3.0.0
SYSTEM_IMAGE_NAME := ${SYSTEM_ID}-v${SYSTEM_VERSION_STRING}

ifeq (${RELEASE},)
  SYSTEM_IMAGE_ID := ${SYSTEM_IMAGE_NAME}-${DATE}-${PLATFORM}
else
  SYSTEM_IMAGE_ID := ${SYSTEM_IMAGE_NAME}-${PLATFORM}
endif


# Meta-targets

all: bootinit packages images
docker: build-docker-staging

images: build-docker-staging
packages: strip-binaries medic-core-pkg medic-rdbms-pkg medic-couch2pg-pkg horticulturalist-pkg medic-api-pkg medic-sentinel-pkg system-services-pkg


# Public targets

bootinit:
	@(cd initrd/common && tar -cf - boot | xz -c) \
	  > ../output/"${SYSTEM_IMAGE_NAME}-${DATE}-all-bootinit.tar.xz"

build: build-start build-finish

copy:
	@(cd source && \
	  ${QMAKE} copy PLATFORM='${PLATFORM}')

clean:
	rm -rf output/* && \
	rm -rf staging/tarball staging/docker

distclean: clean clean-images clean-staging clean-source


# Private targets

build-start:
	@(cd source && \
	  ${QMAKE} build PLATFORM='${PLATFORM}')

build-finish:
	@(cd source && \
	  ${QMAKE} copy finalize PLATFORM='${PLATFORM}')

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

strip-binaries:
	@echo -n 'Removing unnecessary symbols... ' && \
	./scripts/strip-binaries packages && \
	echo 'done.'

horticulturalist-pkg:
	@echo -n 'Compressing package 'horticulturalist'... ' && \
	scripts/build-package 'horticulturalist' 01260 '${PLATFORM}' && \
	echo 'done.'

medic-api-pkg:
	@echo -n 'Compressing package 'medic-api'... ' && \
	scripts/build-package 'medic-api' 00101 '${PLATFORM}' && \
	echo 'done.'

medic-core-pkg:
	@echo -n 'Compressing package 'medic-core'... ' && \
	scripts/build-package 'medic-core' 30000 '${PLATFORM}' && \
	echo 'done.'

medic-couch2pg-pkg:
	@echo -n 'Compressing package 'medic-couch2pg'... ' && \
	scripts/build-package 'medic-couch2pg' 10000 '${PLATFORM}' && \
	echo 'done.'

medic-rdbms-pkg:
	@echo -n 'Compressing package 'medic-rdbms'... ' && \
	scripts/build-package 'medic-rdbms' 94122 '${PLATFORM}' && \
	echo 'done.'

medic-sentinel-pkg:
	@echo -n 'Compressing package 'medic-sentinel'... ' && \
	scripts/build-package 'medic-sentinel' 00101 '${PLATFORM}' && \
	echo 'done.'

system-services-pkg:
	@echo -n 'Compressing package 'system-services'... ' && \
	scripts/build-package 'system-services' 30000 '${PLATFORM}' && \
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

