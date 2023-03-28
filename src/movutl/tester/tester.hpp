#pragma once
#include <iostream>
#include <filesystem>

// clang-format off
#define STR2(x) #x
#define STR(x) STR2(x)

#define TOK2(a, b) a ## b
#define TOK(a, b) TOK2(a, b)

#define TEST_ERROR_PREFIX_ (std::filesystem::path(__FILE__).filename().string() + " @ " +  STR(__LINE__) + " : ")

#define TEST_INFO_COL_PRESIX ""
#define TEST_DEBUG_COL_PRESIX "\x1b[32m"
#define TEST_WARN_COL_PRESIX "\x1b[33m"
#define TEST_ERROR_COL_PRESIX "\x1b[31m"
#define TEST_END_COL_PRESIX "\x1b[0m"

#define TEST_ERROR_(message, condtext) do { std::cout << TEST_ERROR_COL_PRESIX<< "TEST FAILED : " << TEST_ERROR_PREFIX_  << condtext <<   "\t" << message << TEST_END_COL_PRESIX << std::endl; } while(0)
#define TEST_PASSED_(message, condtext) do { std::cout << "TEST PASSED : " << TEST_ERROR_PREFIX_  <<  condtext << "\t" << message << TEST_END_COL_PRESIX << std::endl; } while(0)
#define TEST_CHECK_(cond, condtext, message) do { if (!(cond)) TEST_ERROR_(message, condtext); else TEST_PASSED_(message, condtext); } while(0)

#define TEST_OPERATOR(a, b, op1, op2) do{ \
TEST_CHECK_((a) op1 (b), STR(a) " " STR(op1) " " STR(b), STR(a) " " STR(op1) " " STR(b)); \
if(!((a) op1 (b))){ \
std::cout << TEST_DEBUG_COL_PRESIX << "    -- " << STR(a) <<" = " << (a) << TEST_END_COL_PRESIX<<std::endl ;\
std::cout << TEST_DEBUG_COL_PRESIX << "    -- " << STR(b) <<" = " << (b) << TEST_END_COL_PRESIX<<std::endl;\
} \
}while(0) 

#define TEST(cond) TEST_EQ(cond, true)
#define TEST_EQ(a, b) TEST_OPERATOR(a, b, ==, !=)
#define TEST_NEQ(a, b) TEST_OPERATOR(a, b, !=, ==)
#define TEST_GREATER(a, b) TEST_OPERATOR(a, b, >, <=)
#define TEST_GREATER_EQUAL(a, b) TEST_OPERATOR(a, b, >=, <)
#define TEST_LESS(a, b) TEST_OPERATOR(a, b, <, >=)
#define TEST_LESS_EQUAL(a, b) TEST_OPERATOR(a, b, <=, >)

// clang-format on
