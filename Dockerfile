
FROM ubuntu:16.04
MAINTAINER Medic Mobile

RUN mkdir -p /var/empty
RUN echo x64 > /etc/platform
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get -q=2 -y update --fix-missing && \
    apt-get -q=2 -y upgrade && ln -sf /bin/bash /bin/sh

RUN apt-get -q=2 -y install \
     acl apt-utils attr bzip2 busybox curl daemontools diffutils \
     gawk git isc-dhcp-client less man inotify-tools jq libreadline6 \
     libpcre3 libpopt0 lrzip nano net-tools patch psmisc rsync screen \
     strace sudo unattended-upgrades vim xdelta xz-utils xsltproc

RUN groupadd avahi && groupadd couchdb && groupadd nobody && \
    groupadd postgresql && groupadd sshd && groupadd vm

RUN useradd -rd /var/empty -c 'Service - Avahi' -g avahi avahi && \
    useradd -rd /var/empty -c 'Service - CouchDB' -g couchdb couchdb && \
    useradd -rd /var/empty -c 'Service - Postgres' -g postgresql postgresql && \
    useradd -rd /var/empty -c 'Service - Secure Shell' -g sshd sshd && \
    useradd -rd /home/vm -c 'Legacy - VM Login' -g vm -s /bin/bash vm

ADD platform/staging/docker/x64/medic-os-*-docker /

VOLUME [ "/srv" ]
ENTRYPOINT [ "/bin/bash", "-l", "/boot/container-start", "/" ]

