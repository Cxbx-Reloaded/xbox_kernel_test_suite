#include "exception.h"

BOOL GetExceptionStatusEx(NTSTATUS exception_code, NTSTATUS *status, BOOL ret) {
    *status = exception_code;
    return ret;
}
