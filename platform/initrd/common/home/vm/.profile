#!/bin/sh

for pkg_path in /srv/software/*; do
  if [ -d "$pkg_path" ]; then
    local pkg="`basename "$pkg_path"`"
    for dir in bin sbin; do
      export PATH="$PATH:/srv/software/$pkg/current/default/$dir"
    done
  fi
done

export PS1="\u@\h:\w \$ "
export PATH="/boot:$PATH"

