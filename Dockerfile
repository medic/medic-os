
FROM ubuntu:16.04
MAINTAINER Medic Mobile

RUN mkdir -p /var/empty
RUN echo x64 > /etc/platform
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get -q=2 -y update
RUN apt-get -q=2 -y upgrade
RUN ln -sf /bin/bash /bin/sh

RUN apt-get -q=2 -y install \
  apt-utils busybox gawk isc-dhcp-client less net-tools psmisc sudo vim xz-utils

RUN groupadd avahi
RUN groupadd couchdb
RUN groupadd nobody
RUN groupadd postgresql
RUN groupadd sshd
RUN groupadd vm

RUN useradd -rd /var/empty -c 'Service - Avahi' -g avahi avahi
RUN useradd -rd /var/empty -c 'Service - CouchDB' -g couchdb couchdb
RUN useradd -rd /var/empty -c 'Service - Postgres' -g postgresql postgresql
RUN useradd -rd /var/empty -c 'Service - Secure Shell' -g sshd sshd
RUN useradd -rd /home/vm -c 'Legacy - VM Login' -g vm -s /bin/bash vm

ADD platform/staging/docker/x64/medic-os-*-docker /

VOLUME [ "/srv" ]
ENTRYPOINT [ "/bin/bash", "-l", "/boot/container-start", "/" ]

