
# NAME
     checkrestart - help to found processus that need restarting after upgrade

# SYNOPSIS
     checkrestart [-v] [-M core] [-N system] [-W swap]

# DESCRIPTION
     checkrestart is a program designed to help to found processus that need
     restarting after upgrade.

     checkrestart walkthoughts entire file table of the system, searching for
     processus VTEXT node detached from filesystem.

     By default, checkrestart will output pid and executable name of these
     running processes.

     The options are as follows:

     -v      Verbose mode.  checkrestart will additionnally output inode and
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

# HISTORY
     The name checkrestart comes from a similar tool from Debian that relies
     on lsof(1) for achieving the same purpose.

# AUTHORS
     checkrestart was written by Sebastien Marie <semarie@online.fr>.

