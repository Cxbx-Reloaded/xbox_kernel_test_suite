#pragma once

#include "util/output.h"

#define ASSERT_HEADER BOOL test_passed = 1;

#define ASSERT_FOOTER(test_name) \
    if(!test_passed) { \
        print("  Test '%s' FAILED", test_name); \
    } \
    return test_passed;

#define GEN_CHECK_EX(check_var, expected_var, varname, func_line) \
    if(check_var != expected_var) { \
        print( \
            ((sizeof(check_var) > 4) ? \
            "  ERROR(line %d): Expected %s = 0x%llx, Got = 0x%llx" : \
            "  ERROR(line %d): Expected %s = 0x%x, Got = 0x%x") \
            , func_line, varname, expected_var, check_var \
        ); \
        test_passed = 0; \
    }
#define GEN_CHECK(check_var, expected_var, varname) GEN_CHECK_EX(check_var, expected_var, varname, __LINE__)

#define GEN_CHECK_RANGE_EX(check_var, expected_var, size, varname, func_line) \
    if(check_var < expected_var || check_var > expected_var + size) { \
        print( \
            ((sizeof(check_var) > 4) ? \
            "  ERROR(line %d): Expected range %s = 0x%x-0x%llx, Got = 0x%llx" : \
            "  ERROR(line %d): Expected range %s = 0x%x-0x%x, Got = 0x%x") \
            , func_line, varname, expected_var, expected_var + size, check_var \
        ); \
        test_passed = 0; \
    }
#define GEN_CHECK_RANGE(check_var, expected_var, size, varname) GEN_CHECK_RANGE_EX(check_var, expected_var, size, varname, __LINE__)

#define GEN_CHECK_ARRAY_EX(check_var, expected_var, size, varname, func_line) \
    for (unsigned i = 0; i < size; i++) { \
        if (check_var[i] != expected_var[i]) { \
            print( \
                ((sizeof(check_var[i]) > 4) ? \
                "  ERROR(line %d): Expected array %s[%u] = 0x%llx, Got = 0x%llx" : \
                "  ERROR(line %d): Expected array %s[%u] = 0x%x, Got = 0x%x") \
                , func_line, varname, i, expected_var[i], check_var[i] \
            ); \
            test_passed = 0; \
        } \
    }
#define GEN_CHECK_ARRAY(check_var, expected_var, size, varname) GEN_CHECK_ARRAY_EX(check_var, expected_var, size, varname, __LINE__)

#define GEN_CHECK_ARRAY_MEMBER_EX(var, m_check, m_expected, size, varname, func_line) \
    for (unsigned i = 0; i < size; i++) { \
        if (var[i].m_check != var[i].m_expected) { \
            print( \
                ((sizeof(var[i].m_check) > 4) ? \
                "  ERROR(line %d): Expected array %s[%u].%s = 0x%llx, Got %s[%u].%s = 0x%llx" : \
                "  ERROR(line %d): Expected array %s[%u].%s = 0x%x, Got %s[%u].%s = 0x%x") \
                , func_line, varname, i, #m_expected, var[i].m_expected, varname, i, #m_check, var[i].m_check \
            ); \
            test_passed = 0; \
        } \
    }
#define GEN_CHECK_ARRAY_MEMBER(var, m_check, m_expected, size, varname) GEN_CHECK_ARRAY_MEMBER_EX(var, m_check, m_expected, size, varname, __LINE__)
