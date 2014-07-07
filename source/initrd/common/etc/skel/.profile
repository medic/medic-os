# ~/.profile: Executed by Bourne-compatible login Shells.
#
# Path to personal scripts and executables (~/.local/bin).
[ -d "$HOME/.local/bin" ] || mkdir -p "$HOME/.local/bin"
export PATH=$HOME/.local/bin:$PATH

# Environment variables and prompt for Ash SHell
# or Bash. Default is a classic prompt.
#
PS1='\u@\h:\w\$ '
PAGER='less -EM'
MANPAGER='less -isR'

`which editor >/dev/null` && EDITOR=editor || EDITOR=vi

export PS1 PAGER FILEMGR EDITOR MANPAGER

if [ -f "$HOME/.ashrc" ]; then
   export ENV="$HOME/.ashrc"
   . "$HOME/.ashrc"
fi

