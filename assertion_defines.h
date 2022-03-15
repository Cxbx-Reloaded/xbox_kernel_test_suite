#ifndef ASSERTION_DEFINES_H
#define ASSERTION_DEFINES_H

#include "output.h"

#define ASSERT_HEADER BOOL test_passed = 1;

#define ASSERT_FOOTER(test_name) \
    if(test_passed) { \
        print("  Test '%s' PASSED", test_name); \
    } \
    else { \
        print("  Test '%s' FAILED", test_name); \
    } \
    return test_passed;

#define GEN_CHECK(check_var, expected_var, varname) \
    if(check_var != expected_var) { \
        print( \
            "  Expected %s = 0x%x, Got %s = 0x%x", \
            varname, expected_var, varname, check_var \
        ); \
        test_passed = 0; \
    }

#define GEN_CHECK_RANGE(check_var, expected_var, size, varname) \
    if(check_var < expected_var || check_var > expected_var + size) { \
        print( \
            "  Expected range %s = 0x%x-0x%x, Got %s = 0x%x", \
            varname, expected_var, expected_var + size, varname, check_var \
        ); \
        test_passed = 0; \
    }

#endif // ASSERTION_DEFINES_H
