
Package Structure
=================

Medic OS packages are similar in format to Debian-style `.deb` packages: they
consist of a compressed `tar` archive alongside an assortment of package
metadata files, all combined together into a single file using the `ar` archive
utility. For lack of a better abbreviation, packages for Medic OS currently end
in `.vpkg`.

Inside of the Medic OS source tree, packages are currently assembled using the
https://github.com/medic/medic-os/blob/main/platform/scripts/build-package
script.  The enclosing `ar` archive is created using GNU `ar` – specifically,
via `ar -rcs "$package_root_dir"`. The `ar` utility can be found in the GNU
`binutils` package on most systems.

Archive Contents
----------------

Inside of the `ar` archive, a typical Medic OS package contains the following
files:

<dl>
<dt>package.tar.xz</dt>
<dd>The compressed <code>tar</code> payload, containing binaries, configuration
data, and all other files required for the software to function properly. The
archive should contain correct user/group and permission data; paths inside of
this archive should be relative. The files will be extracted beneath
<code>/srv</code> at package installation time.  Packages cannot directly place
files outside of <code>/srv</code> by design; if you need to make changes to
files located on the <code>initrd</code>-based root filesystem, see the section
on package setup scripts below. The <code>gz</code>, <code>bz2</code>, and
<code>lzma</code> compression formats are also supported, but their use is
discouraged for space-efficiency and uniformity reasons.  </dd>

<dt>name</dt>
<dd>The base name of the package, without any version or architecture/platform
information, e.g. <code>medic-core</code>.</dd>

<dt>version</dt>
<dd>The package version. For simplicity purposes, this must be a whole integer
expressed as ASCII text; the file should contain no other characters other than
trailing whitespace. The version number must increase monotonically as the
package is updated.</dd>

<dt>build-timestamp</dt>
<dd>An ISO 8601 timestamp indicating the date and time at which the package was
assembled.</dd>

<dt>platform</dt>
<dd>The platform for which this package is intended. Currently, this must be
either <code>x64</code>, <code>x86</code>, or <code>armv6</code>.</dd>

<dt>
  digest-md5, digest-sha1, digest-sha256, digest-sha512
</dt>
<dd>The cryptographic digest of the <code>tar</code> archive described above,
stored in hexadecimal notation as ASCII text. At least one of these files
<em>must</em> be present.</dd>

<dt>
  signature<br />
</dt>
<dd>A cryptographic signature for the <code>tar</code> archive described above.
This is currently optional, but may be required in a future version.</dd>
</dl>

Service Management Scripts
==========================

Environment
-----------

Every package should have an `env` script; this should be an `ash`-compatible
shell script that, at a minimum, exports the `PACKAGE_ROOT`, `PACKAGE_NAME`,
and `PACKAGE_VERSION` environment variables. The `env` script should be
installed to `/srv/scripts/$PACKAGE_NAME/env`.

For an example of an `env` script, see
[the Medic OS repository](https://github.com/medic/medic-os/blob/main/platform/packages/medic-core/scripts/medic-core/env)
on Github.

The `env` file is not automatically sourced by any of the system scripts;
it is the responsibility of individual setup/init scripts to `source` it.
The `env` file does not need to be executable (i.e. does not need mode
`+x`).


Boot-time Service Setup Scripts
-------------------------------

At boot, and following any package installation operations, the Medic OS
system will execute any scripts beneath `/srv/scripts/$PACKAGE_NAME/setup`
that have mode `o+x`. These scripts bust be `ash`-compatible, and are
largely used to apply changes to the in-memory `initrd` filesystem – e.g.
installing cron jobs, creating/migrating credentials, or preprocessing
configuration files.  Scripts placed beneath this directory should not
have a file extension.

Service Start-up
----------------

Placing an executable script beneath `/srv/scripts/$PACKAGE_NAME/run`
creates a _service_ – a long-running supervised process that is
automatically restarted upon termination, and can be controlled via the
set of management scripts described in
[INTERNALS.md](https://github.com/medic/medic-os/blob/main/INTERNALS.md).

A service script _must_ block for the duration of the service's lifetime,
typically by using the shell's `exec` function. Scripts placed beneath
this directory should not have a file extension; the service name will
exactly match the name of its `run` script. There is no support for
explicit startup ordering or dependency management; startup/init scripts
are currently left to fend for themselves in that regard.

Scheduled Tasks
===============

Medic OS ships with Busybox's `cron`, preconfigured to automatically run
scripts in any of several `.d` style directories – `cron.always`,
`cron.daily`, `cron.hourly`, `cron.monthly`, and `cron.weekly`. The
`cron.always` directory is run every minute and should be used sparingly.
Scripts that are not explicitly marked as executable are ignored.

The `.d` directories for `cron` are located beneath
`/srv/settings/system-services/cron` and `/etc` by default. The `.d`
directories beneath `/etc` are volatile: the contents will be discarded
upon system shutdown. The `.d` directories beneath
`/srv/settings/system-services/cron` are persistent. Both directory
subtrees are included in the system's preconfigured `crontab` file.

To add a new package-associated `cron` job, you _should_ use a setup
script (described above) to copy your job into the appropriate `.d`
directory. Package setup scripts are run at boot; all setup scripts and
cron job scripts must be `ash`-compatible: `bash` is not available.

If you know what you're doing and why, you _may_ alternatively use the
persistent `.d` directories beneath `/srv/settings/system-services/cron`,
copying the job in from your package's `install` script (or by including
it directly in the package's tar archive).

The default crontab configuration is as follows:

```crontab
# Run 'always' cron jobs every minute (use sparingly)
* * * * * /boot/run-directories /etc/cron.always /srv/settings/system-services/cron/cron.always

# Run hourly cron jobs at 30 minutes after the hour:
30 * * * * /boot/run-directories /etc/cron.hourly /srv/settings/system-services/cron/cron.hourly

# Run daily cron jobs at 12:30 every day:
30 12 * * * /boot/run-directories /etc/cron.daily /srv/settings/system-services/cron/cron.daily

# Run weekly cron jobs at 13:00 on the first day of the week:
30 13 * * 0 /boot/run-directories /etc/cron.weekly /srv/settings/system-services/cron/cron.weekly

# Run monthly cron jobs at 13:30 on the first day of the month:
30 13 1 * * /boot/run-directories /etc/cron.monthly /srv/settings/system-services/cron/cron.monthly
```

