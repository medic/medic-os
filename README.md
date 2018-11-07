
About Medic OS
==============

Medic OS is a size-optimized Linux-based operating system, designed specifically
for Medic Mobile's distributed web applications. It currently targets Docker and
is in a prerelease state at this time. Check back soon for release details.

Building Medic OS
=================

Docker
------

This will fork up an Ubuntu 16.04 LTS container and allow you to quickly build the dependencies inside.

```
docker-compose -f ubuntu_build_container.yml up -d
docker exec -it medic-os_build_container_1 /bin/bash
```

Now run:
```
apt-get update
./platform/config/debian/scripts/prepare-system
make
```

Exit container and shut it down (if you wish).

Build the docker image from your terminal:
```
docker build -t <tag> .
```

If you are pushing to AWS ECR, you'll need to have logged in first.
You can fork up the kubectl-authentication container, and run:
```
aws ecr get-login --region=eu-west-2
```
and paste the output into your terminal (docker login cmd) before running `docker push`

Linux
-----

To ensure a successful build, make sure that you're building on Ubuntu 16.04
LTS. Other Debian-like distributions are known to work, but are not officially
supported.

Run the distribution-specific dependency installation script for your
operating system. These scripts are interactive, and will require you to
confirm modifications. On Debian-like operating systems such as Ubuntu, use:

```shell
sudo ./medic-os/platform/config/debian/scripts/prepare-system
```

Finally, download source code for every component of Medic OS and
immediately start the build process, all in one shot:

```shell
cd medic-os && sudo make
```

Medic OS Internals
==================

Filesystem Layout
-----------------

All user data and non-system software is stored on a dedicated persistent
volume beneath `/srv`. This volume contains four major directories:
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

# Copyright

Copyright 2013-2018 Medic Mobile, Inc. <hello@medicmobile.org>

# License

The software is provided under AGPL-3.0. Contributions to this project are accepted under the same license.
