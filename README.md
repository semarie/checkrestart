
# NAME
     checkrestart - help to find processes that need restarting after upgrade

# SYNOPSIS
     checkrestart [-v] [-M core] [-N system] [-W swap]

# DESCRIPTION
     checkrestart is a program designed to help to find processes that need
     restarting after upgrade.

     checkrestart walks thought entire file table of the system, searching for
     processes VTEXT node detached from filesystem.

     By default, checkrestart will output pid and executable name of these
     running processes.

     The options are as follows:

     -v      Verbose mode.  checkrestart will additionally output inode and
             mount-point of unlinked node.

     -M core
             Extract values associated with the name list from the specified
             core instead of the running kernel.

     -N system
             Extract the name list from the specified system instead of the
             running kernel.

     -W swap
             When not using the running kernel, extract swap information from
             the specified file.

     The typical use case is:

     o   starts a long lived program.

     o   later, upgrades your packages via pkg_add -u.

     o   if the program is updated on disk, the running program is still the
         previous one (without security fixes for example).

     o   checkrestart will tell you which processes belong to this category.

# SEE ALSO
     fstat(1)

# HISTORY
     The name checkrestart comes from a similar tool from Debian that relies
     on lsof(1) for achieving the same purpose.

# AUTHORS
     checkrestart was written by Sebastien Marie <semarie@online.fr>.

# CAVEATS
     Only VTEXT nodes are reported by checkrestart.  Some programs using old
     libraries aren't reported due to lack of support in kernel.

