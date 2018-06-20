
About Medic OS
==============

Medic OS is a size-optimized Linux-based operating system, designed specifically
for Medic Mobile's distributed web applications. It currently runs on VMware,
VirtualBox, Hyper-V, Xen (including Amazon EC2 and Openstack), and Linux
chroot/container environments. A version for Docker based upon Ubuntu LTS is
in progress.

Downloading Medic OS
====================

Unsupported (read: alpha) test builds are currently available at
http://dev.medicmobile.org/downloads/medic-os. This section will be updated when
a supported release is available. For more information, please contact Medic Mobile
at http://medicmobile.org.

Using Medic OS
==============

Instance Creation (ISO)
-----------------------

Once you have an ISO image in hand, you can:

1. Boot the ISO in a virtual machine. [VMware Player](https://github.com/medic/medic-docs/blob/master/md/install/mmva-vmware.md), [Workstation](https://github.com/medic/medic-docs/blob/master/md/install/mmva-vbox.md), and [Fusion](https://github.com/medic/medic-docs/blob/master/md/install/mmva-vmware-fusion.md) are
   fully supported; Virtualbox and Hyper-V lack GSM modem features but are otherwise
   supported.

2. Point your browser the the virtual machine's IP address, which should be displayed
    on the virtual machine console. If your DNS resolver supports mDNS (Bonjour, Zeroconf),
    you should be able to visit `http://medic.local` instead of the IP address.

3. Complete the in-browser setup steps, and wait for services to start up.

4. Click _Finish_, then log in to the application using your credentials from step three.

Administrative Access
---------------------

System-level administrative access to a running copy of Medic OS is available via SSH
on port 33696. In ISO distributions, SSH password authentication is available by default;
the administrative user name is `vm`, and the password is identical to what was set in
step three above.

If you are a developer, have physical access to the virtual machine console, and are
attempting to troubleshoot an issue that has rendered the virtual machine inaccessible,
switch to virtual terminal six (Alt-F6 and enter) to regain access.


Building Medic OS
=================

Linux
-----

To ensure a successful build, check to make sure you have the following software
available at compilation time:

  * Linux kernel version 3.10 or higher
  * Version 2.69 or higher of `autoconf`

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

Next, install the appropriate Java Runtime Environment (JRE) and
Java Development Kit (JDK) beneath `medic-os`. To make this easier, we
provide prepackaged JDK/JRE archives. On 64-bit Intel systems, run:

```shell
file='medic-os-java-x64-latest.tar.xz' &&
curl -LO# "https://medic.s3.amazonaws.com/os/source/$file" &&
tar -xvJf "$file"
```

On 32-bit Intel systems, run the same commands, but replace the first line with:

```shell
file='medic-os-java-x86-latest.tar.xz'
```

Finally, download source code for every component of Medic OS, then
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
close. (Issue: https://github.com/medic/medic-os/issues/43)

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
cd platform/source/medic-core &&
  sudo make ROOT=/srv/software/medic-core/v1.6.1/x64
```

We recommend doing this in a `screen` session, as it can take 60-90
minutes to complete even on modern hardware.

When the build process is finished, you'll need to modify your `PATH`
and `DYLD_LIBRARY_PATH` environment variables to point to the new files.
If you'd rather not do this manually, try running the following:

```shell
source medic-os/platform/initrd/common/boot/include/utility &&
prepend_paths /srv/software/medic-core/v1.6.1/x64
```

To start CouchDB as an unprivileged user, you currently need to modify
the ownership of a few directories. This will be automated in a future
release:

```shell
prefix='/srv/software/medic-core/v1.6.1/x64' &&
for dir in var/log var/lib var/run etc; do
  sudo chown -R "`id -un`:`id -gn`" "$prefix/$dir/couchdb";
done
```

Medic OS Internals
==================

This section is a work in progress aimed at explaining the filesystem layout,
boot process, and execution environment of Medic OS.


Boot Process
------------

The Medic OS root directory is an initial ramdisk (`initrd`) that remains
mounted for the lifetime of the system. This is bundled in an `xz`-compressed
`cpio` archive for distribution, and decompressed into memory by the kernel at
boot. The initrd filesystem contains basic configuration and initialization scripts
in `/etc`, and a basic command-line environment from Busybox in `/bin`, `/sbin`,
`/usr/bin`, and `/usr/sbin`.

The system boots via `/init`, which immediately runs the Busybox `/sbin/init`
binary. Busybox init (PID 1) reads `/etc/inittab`, runs the system status script
on tty1, spawns a recovery console on tty6, and then runs `/etc/init.d/rc.startup`.

The `/etc/init.d/rc.startup` script remounts the root filesystem in read-write
mode, starts `udevd`, configures network interfaces (including DHCP), sets the
system hostname, mounts any filesystems containing user data if they exist, and
then runs the `/boot/system-start` script.

The `/boot/system-start` script allocates and formats storage for user data
beneath `/srv` if it hasn't already been initialized, installs and/or updates
any packages that were provided as part of the AMI/ISO image (see
`package-install`, `package-remove`, and `package-update`), then starts any
services provided by those packages (see `supervisor-start` and
`supervisor-stop`).

Packages and Services
---------------------

Medic OS is comprised of a base system and a collection of installable
packages. Each package provides zero or more services. A service is a
process that is automatically started at boot and is supervised by
the Medic OS startup scripts for the lifetime of the system. A service
in Medic OS are defined by a blocking (read: non-daemonized) startup
script beneath `/srv/scripts/$package/run/$service`, where `$package`
is the name of the package and `$service` is the name of a service
within that package.

Filesystem Layout
-----------------

All user data and non-system software is stored on a dedicated persistent ext4
filesystem beneath `/srv`. This filesystem contains four major directories:
`scripts`, `settings`, `software`, `storage`. Beneath each of these, further
directories are organized by package name, then by service name. For example,
a service named `nginx` provided by a package named `medic-core` would likely
have files in `/srv/settings/medic-core/nginx` and `/srv/storage/medic-core/nginx`.

The `scripts` directory contains package-specific scripts, including
installation, removal, setup, and startup scripts. Each package has its own
directory beneath `scripts`. These directories contain an `install` script and a
`remove` script (used during package installation/removal), a `run` directory
(containing a non-daemonizing startup script for each supervised service), an
`env` script (containing common service-specific variables and parameters), and
a `setup` directory (containing scripts that perform service-specific setup
steps at system boot).

The `settings` directory contains service-specific configuration information,
including `.conf` files, `.ini` files, and SSL/TLS certificates. Each package
has its own directory beneath `settings`, and each service has its own directory
beneath that. For example, the CouchDB configuration files can be found in
`/srv/settings/medic-core/couchdb`.

The `software` directory contains package-specific executable code. Each package
has its own subdirectory beneath `software`. These directories are further
divided by version and architecture, if applicable.  Symbolic links are
maintained for a "default" version and architecture. As an example, binaries for
`medic-core` can always be found beneath `/srv/software/medic-core/current/default`;
version `1.6.1` for `x64` can be found at `/srv/software/medic-core/v1.6.1/x64`.

The `storage` directory contains service-specific data that is generated during
normal use of the system. Each package has its own directory beneath `storage`,
and each service has its own directory beneath that. These directories contain
on-disk database files, log files, and other similarly sensitive information.

A fifth directory, `system`, contains files that are not currently intended
to be touched by humans. This includes package installation receipts, metadata
used by the service supervision scripts, temporary copies of configuration files
that are used as part of a templating process, and so on. Documentation must
be improved here.


Service Management
------------------

Services are currently managed using a set of scripts stored beneath `/boot`. This
location may not be ideal and is subject to change, but should remain in the system
`$PATH`.

Most of these scripts must be run as root via `sudo`.


Service Management Scripts
--------------------------

##### `supervisor-inspect`

List all packages and services. For each listed service, provide status information,
uptime, number of restarts, the process ID, and the supervisor process ID.


##### `svc-down <package-name> [service-name]...`

Bring a service down. This will terminate a supervised process, but leave the supervisor
running so that the process may be restarted later. Multiple services within a single
package may be brought down in a single command.


##### `svc-up [ -f | --force ] <package-name> [service-name]...`

Bring a service back up. This tells a supervisor process to start the service it's responsible
for supervising. If `-f` if provided, the command will ignore services that are already up for
purposes of error reporting and/or exit status.


##### `svc-restart <package-name> [service-name]...`

Perform the equivalent of `svc-down` followed by an immediate `svc-up`. This will cause
temporary process termination and (in all likelihood) perceptible downtime.


##### `svc-reload <package-name> <service-name>...`

Send `SIGHUP` to one or more supervised processes, i.e. services. For some services,
this allows configuration changes to be gracefully read/applied without terminating a
process or causing downtime.


##### `svc-disable <package-name> <service-name>...`

Bring a service down and keep it that way. Perform the equivalent of `svc-down`, but
also mark the service as disabled. The service will not be auto-started at boot again
unless `svc-enable` is run.

##### `svc-enable <package-name> <service-name>...`

Reenable a service and start it. This reverses the effects of `svc-disable`, and
ensures that a service will be automatically started at boot.


Service Management Internals
----------------------------

##### `supervisor-start <package-name> [service-name]...`

Start a new supervisor for the service `service-name`, where `service-name` is a service
provided by `package-name`. This starts a new supervisor process; it doesn't control an
already-running supervisor. To do that, use a command like `svc-up` or `svc-down`.


##### `supervisor-stop <package-name> [service-name]...`

Stop supervising `service-name` entirely. This terminates the service and its supervisor
process, and should not be used during normal operation.


##### `supervisor-list <package-name> [status-name]`

List currently-supervised packages or services. If run with no arguments, this command lists
the packages currently being supervised. If a package name is provided, the command lists
the services in that package that are currently being supervised. If a package status is
provided, only packages with that status (e.g. "running") are listed.

