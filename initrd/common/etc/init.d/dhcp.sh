#!/bin/sh
# The DHCP portion is now separated out, in order to not slow the boot down
# only to wait for slow network cards
. /etc/init.d/tc-functions

# This waits until all devices have registered
/sbin/udevadm settle --timeout=5 &>/dev/null

NETDEVICES="$(awk -F: '/eth.:|tr.:/{print $1}' /proc/net/dev 2>/dev/null)"

for DEVICE in $NETDEVICES; do
  ifconfig "$DEVICE" | grep -q "inet addr"
  if [ "$?" != 0 ]; then
    trap 2 3 11
    /sbin/dhcpcd "$DEVICE" -h "`/bin/hostname`" >/dev/null 2>&1 &
    trap "" 2 3 11
    sleep 1
  fi
done
