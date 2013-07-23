#!/bin/sh

for pkg in medic-core vm-tools; do
  for dir in bin sbin; do
    export PATH="$PATH:/srv/software/$pkg/current/default/$dir"
  done
done

export PS1="\u@\h:\w $ "
export PATH="$PATH:/boot"

