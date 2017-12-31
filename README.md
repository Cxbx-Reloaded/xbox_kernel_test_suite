# xbox kernel test suite
Xbox kernel APIs tester written using the opensoure nxdk

This is a tool for testing xbox kernel apis, in particular CxBx-Reloaded kernel implementation.

# HOW TO BUILD:
All you need is nxdk. You can get it here: https://github.com/xqemu/nxdk

Here is a setup guide: https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/wiki/Installing-the-nxdk

# CONFIGURATION FILE:
The configuration file should be called _**"config.txt"**_ and should be placed in the same directory of the xbe.

This is an example of config.txt:

```
is_emu=1

tests=1,25,3,F
```

**PAY ATTENTION:** Every value is treated as hex (eg. 25 is 37 in decimal)

# USEFUL LINKS:
* https://github.com/wine-mirror/wine/tree/master/dlls/ntdll/tests
* https://github.com/wine-mirror/wine/tree/master/dlls/kernel32/tests
* https://github.com/mirror/reactos/tree/master/rostests/apitests/kernel32
* https://github.com/mborgerson/xbtests?files=1

# TODO:
* Complete the test suite with all xbox kernel APIs (fill the stubs)
* Every functions in utils.c should be ported to nxdk (I'll do that later)

ANY HELP IS REALLY WELCOME!
