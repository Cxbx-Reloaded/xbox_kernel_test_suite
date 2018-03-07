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

# BINARIES:
You can download pre-built bins from here: https://github.com/Cxbx-Reloaded/xbox_kernel_test_suite/releases

# USEFUL LINKS:
* https://github.com/wine-mirror/wine/tree/master/dlls/ntdll/tests
* https://github.com/wine-mirror/wine/tree/master/dlls/kernel32/tests
* https://github.com/mirror/reactos/tree/master/rostests/apitests/kernel32
* https://github.com/mborgerson/xbtests?files=1
* https://github.com/wine-mirror/wine/blob/master/include/wine/test.h

# TODO:
* Complete the test suite with all xbox kernel APIs (fill the stubs)
* Organize the code, maybe using folders etc.

ANY HELP IS REALLY WELCOME!
