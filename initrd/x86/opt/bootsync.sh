#!/bin/sh
# put other system startup commands here, the boot process will wait until they complete.
# Use bootlocal.sh for system startup commands that can run in the background 
# and therefore not slow down the boot process.

/usr/bin/sethostname toaster
sysctl -w kernel.grsecurity.grsec_lock=1 &>/dev/null

exec /boot/system-start

