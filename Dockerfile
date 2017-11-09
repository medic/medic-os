
FROM ubuntu:16.04
MAINTAINER Medic Mobile

RUN apt-get -qy update
ADD platform/staging/tarball/x64/medic-os-*-native /os

VOLUME [ "/os/srv" ]
ENTRYPOINT [ "/os/boot/container-start", "/os" ]

