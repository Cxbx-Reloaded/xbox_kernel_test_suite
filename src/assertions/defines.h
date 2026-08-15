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

#define GEN_CHECK_EX(check_var, expected_var, var_name, func_line) \
    if ((check_var) != (expected_var)) { \
        print( \
            ((sizeof(check_var) > 4) ? \
                "  ERROR(line %d): Expected %s = 0x%llX, Got = 0x%llX" : \
                "  ERROR(line %d): Expected %s = 0x%X, Got = 0x%X" \
            ), \
            func_line, var_name, (expected_var), (check_var) \
        ); \
        TEST_FAILED(); \
    } \
    else if (TEST_VERBOSE) { \
        print( \
            ((sizeof(check_var) > 4) ? \
                "  OK(line %d): %s = 0x%llX" : \
                "  OK(line %d): %s = 0x%X" \
            ), \
            func_line, var_name, (check_var) \
        ); \
    }
#define GEN_CHECK(check_var, expected_var, var_name) GEN_CHECK_EX(check_var, expected_var, var_name, __LINE__)

#define GEN_CHECK_RANGE_EX(check_var, expected_var, length, var_name, func_line) \
    if ((check_var) < (expected_var) || (check_var) > (expected_var) + (length)) { \
        print( \
            ((sizeof(check_var) > 4) ? \
                "  ERROR(line %d): Expected range %s = 0x%llX-0x%llX, Got = 0x%llX" : \
                "  ERROR(line %d): Expected range %s = 0x%X-0x%X, Got = 0x%X" \
            ), \
            func_line, var_name, (expected_var), (expected_var) + (length), (check_var) \
        ); \
        TEST_FAILED(); \
    } \
    else if (TEST_VERBOSE) { \
        print( \
            ((sizeof(check_var) > 4) ? \
                "  OK(line %d): range %s = 0x%llX" : \
                "  OK(line %d): range %s = 0x%X" \
            ), \
            func_line, var_name, (check_var) \
        ); \
    }
#define GEN_CHECK_RANGE(check_var, expected_var, length, var_name) GEN_CHECK_RANGE_EX(check_var, expected_var, length, var_name, __LINE__)

#define GEN_CHECK_ARRAY_DIRECT_EX(check_var, expected_var, var_name, index, func_line) \
    if ((check_var) != (expected_var)) { \
        print( \
            ((sizeof((check_var)) > 4) ? \
                "  ERROR(line %d): Expected array %s[%u] = 0x%llX, Got = 0x%llX" : \
                "  ERROR(line %d): Expected array %s[%u] = 0x%X, Got = 0x%X" \
            ), \
            func_line, var_name, (unsigned)index, (expected_var), (check_var) \
        ); \
        TEST_FAILED(); \
    } \
    else if (TEST_VERBOSE) { \
        print( \
            ((sizeof((check_var)) > 4) ? \
                "  OK(line %d): array %s[%u] = 0x%llX" : \
                "  OK(line %d): array %s[%u] = 0x%X" \
            ), \
            func_line, var_name, (unsigned)index, (check_var) \
        ); \
    }
#define GEN_CHECK_ARRAY_DIRECT(check_var, expected_var, var_name, index) \
    GEN_CHECK_ARRAY_DIRECT_EX(check_var, expected_var, var_name, index, __LINE__)

#define GEN_CHECK_ARRAY_INDEX_EX(check_var, expected_var, var_name, index, func_line) \
    GEN_CHECK_ARRAY_DIRECT_EX((check_var)[index], (expected_var)[index], var_name, index, func_line)

#define GEN_CHECK_ARRAY_INDEX(check_var, expected_var, var_name, index) \
    GEN_CHECK_ARRAY_INDEX_EX(check_var, expected_var, var_name, index, __LINE__)

#define GEN_CHECK_ARRAY_EX(check_var, expected_var, size, var_name, func_line) \
    for (unsigned i = 0; i < (size); i++) { \
        GEN_CHECK_ARRAY_INDEX_EX(check_var, expected_var, var_name, i, func_line) \
    }
#define GEN_CHECK_ARRAY(check_var, expected_var, size, var_name) GEN_CHECK_ARRAY_EX(check_var, expected_var, size, var_name, __LINE__)

#define GEN_CHECK_ARRAY_MEMBER_DIRECT_EX(check_var, expected_var, var_name, index, member_name, func_line) \
    if ((check_var) != (expected_var)) { \
        print( \
            ((sizeof((check_var)) > 4) ? \
                "  ERROR(line %d): Expected array member %s[%u].%s = 0x%llX, Got = 0x%llX" : \
                "  ERROR(line %d): Expected array member %s[%u].%s = 0x%X, Got = 0x%X" \
            ), \
            func_line, var_name, (unsigned)index, member_name, (check_var) \
        ); \
        TEST_FAILED(); \
    } \
    else if (TEST_VERBOSE) { \
        print( \
            ((sizeof((check_var)) > 4) ? \
                "  OK(line %d): array member %s[%u].%s = 0x%llX" : \
                "  OK(line %d): array member %s[%u].%s = 0x%X" \
            ), \
            func_line, var_name, (unsigned)index, member_name, (check_var) \
        ); \
    }
#define GEN_CHECK_ARRAY_MEMBER_DIRECT(check_var, expected_var, var_name, index, member_name) \
    GEN_CHECK_ARRAY_MEMBER_DIRECT_EX(check_var, expected_var, var_name, index, member_name, __LINE__)

#define GEN_CHECK_ARRAY_MEMBER_INDEX_EX(var, m_check, m_expected, var_name, index, func_line) \
    GEN_CHECK_ARRAY_MEMBER_DIRECT_EX((var)[index].m_check, (var)[index].m_expected, var_name, index, #m_check, func_line)

#define GEN_CHECK_ARRAY_MEMBER_INDEX(var, m_check, m_expected, var_name, index) \
    GEN_CHECK_ARRAY_MEMBER_INDEX_EX(var, m_check, m_expected, var_name, index, __LINE__)

#define GEN_CHECK_ARRAY_MEMBER_EX(var, m_check, m_expected, size, var_name, func_line) \
    for (unsigned i = 0; i < (size); i++) { \
        GEN_CHECK_ARRAY_MEMBER_INDEX_EX(var, m_check, m_expected, var_name, i, func_line); \
    }
#define GEN_CHECK_ARRAY_MEMBER(var, m_check, m_expected, size, var_name) GEN_CHECK_ARRAY_MEMBER_EX(var, m_check, m_expected, size, var_name, __LINE__)

#define GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT_EX(check_var, expected_var, var_name, index_1, member_name, index_2, func_line) \
    if ((check_var) != (expected_var)) { \
        print( \
            ((sizeof((check_var)) > 4) ? \
                "  ERROR(line %d): Expected member array %s[%u].%s[0x%08X] = 0x%llX, Got = 0x%llX" : \
                "  ERROR(line %d): Expected member array %s[%u].%s[0x%08X] = 0x%X, Got = 0x%X" \
            ), \
            func_line, var_name, (unsigned)index_1, member_name, (unsigned)index_2, (expected_var), (check_var) \
        ); \
        TEST_FAILED(); \
    } \
    else if (TEST_VERBOSE) { \
        print( \
            ((sizeof((check_var)) > 4) ? \
                "  OK(line %d): member array %s[%u].%s[0x%08X] = 0x%llX" : \
                "  OK(line %d): member array %s[%u].%s[0x%08X] = 0x%X" \
            ), \
            func_line, var_name, (unsigned)index_1, member_name, (unsigned)index_2, (check_var) \
        ); \
    }
#define GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT(check_var, expected_var, var_name, index_1, member_name, index_2) \
    GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT_EX(check_var, expected_var, var_name, index_1, member_name, index_2, __LINE__)

#define GEN_CHECK_ARRAY_MEMBER_ARRAY_EX(var, m_check, m_expected, size_1, size_2, var_name, func_line) \
    for (unsigned i = 0; i < (size_1); i++) { \
        for (unsigned ii = 0; ii < (size_2); ii++) { \
            GEN_CHECK_ARRAY_MEMBER_ARRAY_DIRECT_EX((var)[i].m_check[ii], (var)[i].m_expected[ii], var_name, i, #m_check, ii, func_line); \
        } \
    }
#define GEN_CHECK_ARRAY_MEMBER_ARRAY(var, m_check, m_expected, size_1, size_2, var_name) \
    GEN_CHECK_ARRAY_MEMBER_ARRAY_EX(var, m_check, m_expected, size_1, size_2, var_name, __LINE__)

#define GEN_CHECK_NESTED_MEMBER_DIRECT_EX(check_var, expected_var, var_name, index_1, index_2, member_name, func_line) \
    if ((check_var) != (expected_var)) { \
        print( \
            ((sizeof((check_var)) > 4) ? \
                "  ERROR(line %d): Expected nested member %s[%u][0x%08X].%s = 0x%llX, Got = 0x%llX" : \
                "  ERROR(line %d): Expected nested member %s[%u][0x%08X].%s = 0x%X, Got = 0x%X" \
            ), \
            func_line, var_name, (unsigned)index_1, (unsigned)index_2, member_name, (expected_var), (check_var) \
        ); \
        TEST_FAILED(); \
    } \
    else if (TEST_VERBOSE) { \
        print( \
            ((sizeof((check_var)) > 4) ? \
                "  OK(line %d): nested member %s[%u][0x%08X].%s = 0x%llX" : \
                "  OK(line %d): nested member %s[%u][0x%08X].%s = 0x%X" \
            ), \
            func_line, var_name, (unsigned)index_1, (unsigned)index_2, member_name, (check_var) \
        ); \
    }
#define GEN_CHECK_NESTED_MEMBER_DIRECT(check_var, expected_var, var_name, index_1, index_2, member_name) \
    GEN_CHECK_NESTED_MEMBER_DIRECT_EX(check_var, expected_var, var_name, index_1, index_2, member_name, __LINE__)

#define GEN_CHECK_NESTED_MEMBER_EX(var, m_check, m_expected, size_1, size_2, var_name, func_line) \
    for (unsigned i = 0; i < (size_1); i++) { \
        for (unsigned ii = 0; ii < (size_2); ii++) { \
            GEN_CHECK_NESTED_MEMBER_DIRECT_EX((var)[i][ii].m_check, (var)[i][ii].m_expected, var_name, i, ii, #m_check, func_line); \
        } \
    }
#define GEN_CHECK_NESTED_MEMBER(var, m_check, m_expected, size_1, size_2, var_name) \
    GEN_CHECK_NESTED_MEMBER_EX(var, m_check, m_expected, size_1, size_2, var_name, __LINE__)
