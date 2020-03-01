//#include "ctest.h"
#include "utils/utils.h"

void stringSplit_emptyTokens() {
    char str[] = "aaa";
    char *tokens[4];
//    string_split(str, tokens, "a");

//    for (int i = 0; i < 4; ++i)
//        ASSERT_STR(tokens[i], "");
}

void stringSplit_singleToken() {
    char str[] = "aaa";
    char *tokens[4];
//    string_split(str, tokens, "b");

//    ASSERT_STR(tokens[0], "aaa");
//    for (int i = 1; i < 4; ++i)
//        ASSERT_STR(tokens[i], "");
}

void stringSplit_singleToken2() {
    char str[] = "baa";
    char *tokens[4];
//    string_split(str, tokens, "b");

//    ASSERT_STR(tokens[0], "");
//    ASSERT_STR(tokens[1], "aa");
//    for (int i = 2; i < 4; ++i)
//        ASSERT_STR(tokens[i], "");
}

void test_utils_runner() {
    stringSplit_emptyTokens();
    stringSplit_singleToken();
    stringSplit_singleToken2();
}