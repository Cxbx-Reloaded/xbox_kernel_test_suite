# xbox kernel test suite
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://github.com/Cxbx-Reloaded/xbox_kernel_test_suite/blob/master/LICENSE)
[![GitHub CI](https://github.com/Cxbx-Reloaded/xbox_kernel_test_suite/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/Cxbx-Reloaded/xbox_kernel_test_suite/actions/workflows/ci.yml?query=branch%3Amaster)

Xbox kernel APIs tester written using the open-source nxdk.

This is a tool for testing the xbox kernel APIs on the hardware to verify tests are passing. Plus we have a dedicated wiki page for [homebrew kernels](https://github.com/Cxbx-Reloaded/xbox_kernel_test_suite/wiki/List-of-Homebrew-Kernels-Tested) have used this tool to test their kernel implementation.

## HOW TO BUILD:
All you need is nxdk. You can get it here: https://github.com/XboxDev/nxdk

Here is a setup guide: https://github.com/XboxDev/nxdk/wiki/Getting-Started

## CONFIGURATION FILE:
> [!NOTE]
> The configuration file should be called _**"config.txt"**_ and should be placed in the same directory of the xbe.

The following list of options can be used inside config.txt file:
- `seed` = `<hexadecimal (support up to unsigned int max)>`
- `tests` = `<hexadecimal (support up to 17A)>[,...]`
- `tests-exclude` = `<hexadecimal (support up to 17A)>[,...]`
- `disable-video` = `<boolean>`[^1]

[^1]: boolean value can be 1 or 0

> [!TIP]
> This is an example of config.txt:
> ```
> seed=5
> 
> tests=1,25,3,F
> ```

## NAME FILE:
> [!NOTE]
> The name file should be called _**"name.txt"**_ and should be placed in the same directory of the xbe.

Having a name file can help generate individual log files for different hardware and emulators to collect results in the same folder without the need to rename log files every time.

> [!TIP]
> This is an example of name.txt:
> ```
> retail 1.6
> ```

## BINARIES:
You can download pre-built bins from here: https://github.com/Cxbx-Reloaded/xbox_kernel_test_suite/releases

## USEFUL LINKS:
* https://xboxdevwiki.net
* https://github.com/wine-mirror/wine/tree/master/dlls/ntdll/tests
* https://github.com/wine-mirror/wine/tree/master/dlls/kernel32/tests
* https://github.com/mirror/reactos/tree/master/rostests/apitests/kernel32
* https://github.com/mborgerson/xbtests?files=1
* https://github.com/wine-mirror/wine/blob/master/include/wine/test.h

## TODO:
* Complete the test suite with all xbox kernel APIs (fill in the FIXME stubs).
* Check the [opened issues](https://github.com/Cxbx-Reloaded/xbox_kernel_test_suite/issues) for any issues or tasks.

ANY HELP IS REALLY WELCOME!
