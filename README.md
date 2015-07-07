
About Medic OS
==============

Medic OS is a size-optimized Linux-based operating system, designed specifically
for Medic Mobile's distributed web applications. It currently runs on VMware,
VirtualBox, Hyper-V, Xen (including Amazon EC2 and Openstack), and Linux
chroot/container environments. A port for ARM v6 and ARM v7 machines (such
as the Raspberry Pi and Beaglebone Black) is in progress.

Medic OS provides a Busybox-based userspace environment, Erlang runtime,
CouchDB, Node.js, OpenSSH, a Java runtime environment, couchdb-lucene, Gammu,
gammu-json, and a number of other smaller utilities and modules. The full
compressed Medic OS distribution is less than one hundred megabytes.

Downloading Medic OS
====================

Unsupported (read: alpha) test builds are currently available at
http://dev.medicmobile.org/downloads/medic-os. This section will be updated when
a supported release is available. For more information, please contact Medic Mobile
at http://medicmobile.org.


Building Medic OS
=================

Linux
-----

Check out a copy of the Medic OS source repository:

```shell
git clone 'https://github.com/medic/medic-os.git'
```

Ensure that you're running Amazon Linux or Ubuntu 14.04 LTS. Other
Redhat-like and Debian-like distributions are known to work, but are
not officially supported. Slackware works (and tagfiles are provided),
but you may need to manually upgrade `automake` and/or `autoconf`.

Run the distribution-specific dependency installation script for your
operating system. These scripts are interactive, and will require you to
confirm modifications. On Redhat-like operating systems, use:

```shell
sudo ./medic-os/platform/config/redhat/scripts/prepare-system
```

On Debian-like operating systems, use:

```shell
sudo ./medic-os/platform/config/debian/scripts/prepare-system
```

Download source code for every component of Medic OS, and then
immediately start the build process, all in one shot:

```shell
cd medic-os && sudo make
```

Mac OS X
--------

**Warning**: Developers only.

Ensure that you have the necessary build-time dependencies. There is
currently no definitive list for Mac OS X, but dependencies are
documented for Redhat-like and Debian-like Linux distributions. A
Homebrew or Fink environment with a working GCC toolchain should get you
close.

Check out a copy of the Medic OS source repository:

```shell
git clone 'https://github.com/medic/medic-os.git'
```

Download source code for every component of Medic OS – this may take
some time on a slow connection. This must be done as root currently, but
we're working to remove this requirement in a future version:

```shell
cd medic-os && sudo make download
```

Start the build process. The following command builds a supported subset
of Medic OS – specifically, the `medic-core` package – but not entire
operating system images or containers. This mode of use is intended for
developers and/or testers, and is not suitable for production
environments:

```shell
cd platform/source/medic-os &&
  sudo make ROOT=/srv/software/medic-core/v1.6.1/x64
```

We recommend doing this in a `screen` session, as it can take 60-90
minutes to complete even on modern hardware.
