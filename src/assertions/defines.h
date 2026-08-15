#pragma once

#include "util/output.h"

#define TEST_VERBOSE get_verbose_value()

#define ASSERT_HEADER(test_name) \
    BOOL test_passed = 1; \
    if (TEST_VERBOSE) { \
        print("  Test '%s' Starting", test_name); \
    }

#define ASSERT_FOOTER(test_name) \
    if (!test_passed) { \
        print("  Test '%s' FAILED", test_name); \
    } \
    else if (TEST_VERBOSE) { \
        print("  Test '%s' PASSED", test_name); \
    } \
    return test_passed

#define GEN_CHECK_EX(check_var, expected_var, varname, func_line) \
    if ((check_var) != (expected_var)) { \
        print( \
            ((sizeof(check_var) > 4) ? \
                "  ERROR(line %d): Expected %s = 0x%llX, Got = 0x%llX" : \
                "  ERROR(line %d): Expected %s = 0x%X, Got = 0x%X" \
            ), \
            func_line, varname, (expected_var), (check_var) \
        ); \
        TEST_FAILED(); \
    } \
    else if (TEST_VERBOSE) { \
        print( \
            ((sizeof(check_var) > 4) ? \
                "  OK(line %d): %s = 0x%llX" : \
                "  OK(line %d): %s = 0x%X" \
            ), \
            func_line, varname, (check_var) \
        ); \
    }
#define GEN_CHECK(check_var, expected_var, varname) GEN_CHECK_EX(check_var, expected_var, varname, __LINE__)

#define GEN_CHECK_RANGE_EX(check_var, expected_var, length, varname, func_line) \
    if ((check_var) < (expected_var) || (check_var) > (expected_var) + (length)) { \
        print( \
            ((sizeof(check_var) > 4) ? \
                "  ERROR(line %d): Expected range %s = 0x%llX-0x%llX, Got = 0x%llX" : \
                "  ERROR(line %d): Expected range %s = 0x%X-0x%X, Got = 0x%X" \
            ), \
            func_line, varname, (expected_var), (expected_var) + (length), (check_var) \
        ); \
        TEST_FAILED(); \
    } \
    else if (TEST_VERBOSE) { \
        print( \
            ((sizeof(check_var) > 4) ? \
                "  OK(line %d): range %s = 0x%llX" : \
                "  OK(line %d): range %s = 0x%X" \
            ), \
            func_line, varname, (check_var) \
        ); \
    }
#define GEN_CHECK_RANGE(check_var, expected_var, length, varname) GEN_CHECK_RANGE_EX(check_var, expected_var, length, varname, __LINE__)

#define GEN_CHECK_ARRAY_DIRECT_EX(check_var, expected_var, varname, index, func_line) \
    if ((check_var) != (expected_var)) { \
        print( \
            ((sizeof((check_var)) > 4) ? \
                "  ERROR(line %d): Expected array %s[%u] = 0x%llX, Got = 0x%llX" : \
                "  ERROR(line %d): Expected array %s[%u] = 0x%X, Got = 0x%X" \
            ), \
            func_line, varname, (unsigned)index, (expected_var), (check_var) \
        ); \
        TEST_FAILED(); \
    } \
    else if (TEST_VERBOSE) { \
        print( \
            ((sizeof((check_var)) > 4) ? \
                "  OK(line %d): array %s[%u] = 0x%llX" : \
                "  OK(line %d): array %s[%u] = 0x%X" \
            ), \
            func_line, varname, (unsigned)index, (check_var) \
        ); \
    }
#define GEN_CHECK_ARRAY_DIRECT(check_var, expected_var, varname, index) \
    GEN_CHECK_ARRAY_DIRECT_EX(check_var, expected_var, varname, index, __LINE__)

#define GEN_CHECK_ARRAY_INDEX_EX(check_var, expected_var, varname, index, func_line) \
    GEN_CHECK_ARRAY_DIRECT_EX((check_var)[index], (expected_var)[index], varname, index, func_line)

#define GEN_CHECK_ARRAY_INDEX(check_var, expected_var, varname, index) \
    GEN_CHECK_ARRAY_INDEX_EX(check_var, expected_var, varname, index, __LINE__)

#define GEN_CHECK_ARRAY_EX(check_var, expected_var, size, varname, func_line) \
    for (unsigned i = 0; i < (size); i++) { \
        GEN_CHECK_ARRAY_INDEX_EX(check_var, expected_var, varname, i, func_line) \
    }
#define GEN_CHECK_ARRAY(check_var, expected_var, size, varname) GEN_CHECK_ARRAY_EX(check_var, expected_var, size, varname, __LINE__)

#define GEN_CHECK_ARRAY_MEMBER_DIRECT_EX(check_var, expected_var, varname, index, member_name, func_line) \
    if ((check_var) != (expected_var)) { \
        print( \
            ((sizeof((check_var)) > 4) ? \
                "  ERROR(line %d): Expected array member %s[%u].%s = 0x%llX, Got = 0x%llX" : \
                "  ERROR(line %d): Expected array member %s[%u].%s = 0x%X, Got = 0x%X" \
            ), \
            func_line, varname, (unsigned)index, member_name, (check_var) \
        ); \
        TEST_FAILED(); \
    } \
    else if (TEST_VERBOSE) { \
        print( \
            ((sizeof((check_var)) > 4) ? \
                "  OK(line %d): array member %s[%u].%s = 0x%llX" : \
                "  OK(line %d): array member %s[%u].%s = 0x%X" \
            ), \
            func_line, varname, (unsigned)index, member_name, (check_var) \
        ); \
    }
#define GEN_CHECK_ARRAY_MEMBER_DIRECT(check_var, expected_var, varname, index, member_name) \
    GEN_CHECK_ARRAY_MEMBER_DIRECT_EX(check_var, expected_var, varname, index, member_name, __LINE__)

#define GEN_CHECK_ARRAY_MEMBER_INDEX_EX(var, m_check, m_expected, varname, index, func_line) \
    GEN_CHECK_ARRAY_MEMBER_DIRECT_EX((var)[index].m_check, (var)[index].m_expected, varname, index, #m_check, func_line)

#define GEN_CHECK_ARRAY_MEMBER_INDEX(var, m_check, m_expected, varname, index) \
    GEN_CHECK_ARRAY_MEMBER_INDEX_EX(var, m_check, m_expected, varname, index, __LINE__)

#define GEN_CHECK_ARRAY_MEMBER_EX(var, m_check, m_expected, size, varname, func_line) \
    for (unsigned i = 0; i < (size); i++) { \
        GEN_CHECK_ARRAY_MEMBER_INDEX_EX(var, m_check, m_expected, varname, i, func_line); \
    }
#define GEN_CHECK_ARRAY_MEMBER(var, m_check, m_expected, size, varname) GEN_CHECK_ARRAY_MEMBER_EX(var, m_check, m_expected, size, varname, __LINE__)
