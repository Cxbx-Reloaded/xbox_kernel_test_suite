#pragma once

#include <excpt.h>

BOOL GetExceptionStatusEx(NTSTATUS exception_code, NTSTATUS *status, BOOL ret);

#define GetExceptionStatus(status, ret) GetExceptionStatusEx(GetExceptionCode(), status, ret)
