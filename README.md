This is the example from:

https://stackoverflow.com/a/20082113/262458

, just run `make` in one of the MSYS2 native build shells or an MSYS shell.
Should also work on Cygwin with the relevant packages installed.

The resulting executable is `show-parent.exe`, which takes an optional argument
for the pid to look up, otherwise it will use its own pid.
