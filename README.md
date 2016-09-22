this software is still **experimental**

the purpose is to have a way to know when a running program is using an old version of upgraded files.

the typical use case is:
  - starts a long lived program
  - later, upgrades your packages via `pkg_add -u`
  - if the program is updated on disk, the running program is still the previous one (without security fixes for example).

`checkrestart` will tell you:
  - the pid of the running program
  - the program name (or a hint about it...)
  - the inode of the unlinked file
  - the mount point of the unlinked file

this information should be enough to guess how to restart the program.

it *should* works as, under OpenBSD, when a library is updated, all packages with dependencies on it will have their signature changed, and programs will been reinstalled too.

the name was taken from [checkrestart](https://packages.debian.org/sid/debian-goodies) program from Debian, which relies on `lsof` for the same purpose.
